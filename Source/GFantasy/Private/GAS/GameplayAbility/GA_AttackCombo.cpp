#include "GAS/GameplayAbility/GA_AttackCombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"

UGA_AttackCombo::UGA_AttackCombo()
{
	// 인스턴스 방식 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//해당 어빌리티에 태그 부여
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Combo")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Combo")));


}

void UGA_AttackCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKismetSystemLibrary::PrintString(this, TEXT("Attack Combo Start"));

	//리소스 소비 쿨다운
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Cool Down End"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	//기본 변수 초기화
	ComboIndex = 0;
	bCanCombo = false;
	bIsMontageDone = false;
	bIsInputReleased = false;
	bHoldTriggered = false;
	HeldTime = 0.f;
	CurrentMontageTask = nullptr;

	// 콤보 윈도우 준비
	SetupComboWindow();

	ResetRotation();

	/* 차징 여부 */

	// 입력 시간 초기화
	HeldTime = 0.f;
	bHoldTriggered = false;
	bIsInputReleased = false;

	// 0.05초마다 UpdateHold 실행
	GetWorld()->GetTimerManager().SetTimer(
		HoldTH,
		this,
		&UGA_AttackCombo::UpdateHold,
		0.05f,
		true	//반복여부
	);

	// 첫 번째 콤보 공격 실행
	PlayComboStep(ComboIndex);
}

// 어빌리티에서의 입력 어빌리티로 설정된 경우, 입력이 들어오면 아래의 함수가 실행된다.
void UGA_AttackCombo::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	// 콤보 윈도우가 열려있지 않으면 무시.

	if (!bCanCombo )
	{
		return;
	}

	// 서버 / 클라 콤보 실행.
	if (HasAuthority(&ActivationInfo) || IsPredictingClient())
	{
		// 서버에서 실행 
		ComboAttack();	//몽타주 실행하고
		UE_LOG(LogTemp, Warning, TEXT("AttackCombo : 1"));
	}
	if (!HasAuthority(&ActivationInfo))
	{
		// 클라일 경우, 서버 RPC 실행하기.
		ActorInfo->AbilitySystemComponent->ServerSetInputPressed(Handle);
		UE_LOG(LogTemp, Warning, TEXT("AttackCombo : 2"));
	}
}


// 콤보 공격 실행 
void UGA_AttackCombo::ComboAttack()
{
	if (!bCanCombo)
	{
		//현재 콤보 공격이 가능한 상태가 아니라면
		return;
	}

	// 이전 Task 초기화
	SafeEndCurrentMontageTask();

	// 콤보 인덱스 증가 및 상태 초기화.
	++ComboIndex;
	bCanCombo = false;
	bIsMontageDone = false;

	// 이전 콤보 윈도우 태스크 종료
	ClearComboWindowTasks();

	// 만약 락온 상태일 시, 타겟 바라보게 회전.
	RotateToLockOnTaret();

	// comboIndex에 따른 공격 실행
	PlayComboStep(ComboIndex);
}

void UGA_AttackCombo::SetupComboWindow()
{
	CanComboTrueTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Combo.InputWindow.Start"),
		nullptr,
		true,
		false
	);
	CanComboTrueTask->EventReceived.AddDynamic(this, &UGA_AttackCombo::CanComboTrue);
	CanComboTrueTask->ReadyForActivation();

	CanComboFalseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Combo.InputWindow.End"),
		nullptr,
		true,
		false
	);
	CanComboFalseTask->EventReceived.AddDynamic(this, &UGA_AttackCombo::CanComboFalse);
	CanComboFalseTask->ReadyForActivation();
}

void UGA_AttackCombo::ClearComboWindowTasks()
{
	if (CanComboTrueTask)
	{
		CanComboTrueTask->EndTask();
		CanComboTrueTask = nullptr;
	}
	if (CanComboFalseTask)
	{
		CanComboFalseTask->EndTask();
		CanComboFalseTask = nullptr;
	}
}

void UGA_AttackCombo::SafeEndCurrentMontageTask()
{
	if (CurrentMontageTask && IsValid(CurrentMontageTask))
	{
		// 델리게이트 바인딩 해제
		CurrentMontageTask->OnCompleted.RemoveAll(this);
		CurrentMontageTask->OnCancelled.RemoveAll(this);
		CurrentMontageTask->OnInterrupted.RemoveAll(this);

		// 태스크 종료
		CurrentMontageTask->EndTask();
		CurrentMontageTask = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("SafeEndCurrentMontageTask Success"));
	}
}

void UGA_AttackCombo::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	bIsInputReleased = true;

	// 타이머 중단
	GetWorld()->GetTimerManager().ClearTimer(HoldTH);

	UE_LOG(LogTemp, Warning, TEXT("InputReleased"));


	// 홀드가 트리거되었다면 차징 스킬 실행


	if (bHoldTriggered)
	{
		// 일정 시간 이상 눌러졌으면 -> 커맨드 스킬 실행
		// 어빌리티 태그 가져와서 해당 어빌리티 실행

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

		if (ASC)
		{

			// Tag 기반 Effect & Cue 제거
			if (HasAuthority(&CurrentActivationInfo))
			{
				if (ChargingEffectHandle.IsValid())
				{
					ASC->RemoveActiveGameplayEffect(ChargingEffectHandle);
					ChargingEffectHandle = FActiveGameplayEffectHandle();
					UKismetSystemLibrary::PrintString(this, TEXT("ChargingEffectHandle Valid"));
				}


			}
			else
			{
				ActorInfo->AbilitySystemComponent->ServerSetInputReleased(Handle);
				//Server_ReleasedInput();
				
				UKismetSystemLibrary::PrintString(this, TEXT("Charge Effect In Client"));
			}

		}

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

		UPlayerCharacterGASC* PlayerASC = Cast<UPlayerCharacterGASC>(ASC);
		UKismetSystemLibrary::PrintString(this, TEXT("Remove Charge Effect In Server"));
		FGameplayTag ChargeTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Skill.Sword.ChargeCommand"));

		// ASC에게 어빌리티 넘기기 해당 함수에서 어빌리티 실행
		FTimerDelegate Delegate;
		Delegate.BindUFunction(PlayerASC, FName("DeferredActivateAbilityByTag"), ChargeTag);

		GetWorld()->GetTimerManager().SetTimer(
			TempHandle, Delegate, 0.05f, false
		);
		//GetWorld()->GetTimerManager().SetTimerForNextTick(Delegate);
		
		return;
	}

	// 차징 미발동 / 몽타주가 이미 끝난 상태 -> 즉시 종료.
	if (bIsMontageDone && !bHoldTriggered)
	{
		// 아니면 그냥 일반 공격 종료
		UE_LOG(LogTemp, Warning, TEXT("AttackCombo : 4"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

	if (!bIsMontageDone && !bHoldTriggered)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputReleased but montage not yet done"));
	}

}

void UGA_AttackCombo::OnMontageCompleted()
{
	// 현재 어빌리티 실행중인 캐릭터 가져오기
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Player)
		return;

	bIsMontageDone = true;

	// 입력이 이미 떼어져 있으면 즉시 종료.
	if (bIsInputReleased && !bHoldTriggered)
	{
		UE_LOG(LogTemp, Warning, TEXT("bHoldTriggered : false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// if (bHoldTriggered)
	// {
	// 	return;
	// }
	//
	// // 입력이 떼어진 상태든 아니든 종료하기.
	// EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}

void UGA_AttackCombo::OnMontageCanceled()
{
	//ResetRotation();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// 2~4단 콤보 몽타주 실행
void UGA_AttackCombo::PlayComboStep(int32 NewComboIndex)
{

	// 배열 범위를 벗어나면 (마지막 스텝을 이미 넘겼으면 ) -> OnMontageCompleted로 간주
	if (!ComboMontages.IsValidIndex(NewComboIndex))
	{
		OnMontageCompleted();
		return;
	}

	SetupComboWindow();

	CurrentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ComboMontages[NewComboIndex]
	);

	if (CurrentMontageTask)
	{
		CurrentMontageTask->OnCompleted.AddDynamic(this, &UGA_AttackCombo::OnMontageCompleted);
		CurrentMontageTask->OnCancelled.AddDynamic(this, &UGA_AttackCombo::OnMontageCanceled);
		CurrentMontageTask->OnInterrupted.AddDynamic(this, &UGA_AttackCombo::OnMontageCanceled);
		CurrentMontageTask->ReadyForActivation();

		UE_LOG(LogTemp, Warning, TEXT("CurrentMontageTask STEP %d"), NewComboIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No CurrentMontageTask"));
		OnMontageCompleted();
	}
}



void UGA_AttackCombo::CanComboTrue(FGameplayEventData Payload)
{
	bCanCombo = true;

}

void UGA_AttackCombo::CanComboFalse(FGameplayEventData Payload)
{
	bCanCombo = false;
}

void UGA_AttackCombo::ReleasedInput()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(
		ChargingCueTag
	);
}

void UGA_AttackCombo::ExecuteChargeAbility()
{
	const FGameplayTag ChargeTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Skill.Sword.ChargeCommand"));

	FGameplayTagContainer Tags;
	Tags.AddTag(ChargeTag);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	if (!ASC)	return;
	if (HasAuthority(&CurrentActivationInfo))
	{
		const bool bSuccess = ASC->TryActivateAbilitiesByTag(Tags);
		if (!bSuccess)
		{
			UE_LOG(LogTemp, Error, TEXT("Charge Ability Not Activated!"));
		}
	}

}

void UGA_AttackCombo::ResetRotation()

{	// 현재 어빌리티 실행중인 캐릭터 가져오기
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Player)
		return;

	const FVector InputDir = Player->CachedInputDirection;

	if (Player->TargetingState == ETargetingState::LockOn && Player->CurrentTarget)
	{
		FVector ToTarget = (Player->CurrentTarget->GetActorLocation() - Player->GetActorLocation().
			GetSafeNormal2D());
		FRotator LookAtRot = ToTarget.Rotation();

		Player->SetActorRotation(LookAtRot);

		if (Player->Controller)
		{
			//Player->Controller->SetControlRotation(LookAtRot);
		}

	}

	if (!InputDir.IsNearlyZero())
	{

		const FRotator DirRot = InputDir.Rotation();
		Player->SetActorRotation(DirRot);
	}

	UE_LOG(LogTemp, Warning, TEXT("Current Rot : %s"), *InputDir.ToString());

}

bool UGA_AttackCombo::IsInputStillPressed()
{
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return Spec && Spec->InputPressed;
}

void UGA_AttackCombo::UpdateHold()
{
	if (bHoldTriggered || ComboIndex > 0)
	{
		return;
	}

	HeldTime += 0.05f;

	UE_LOG(LogTemp, Warning, TEXT("UpdateHold HeldTime : %f"), HeldTime);

	if (HeldTime >= HoldThreshold)
	{

		if (HasAuthority(&CurrentActivationInfo))
		{
			UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

			if (ChargingEffectClass && ASC)
			{
				FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
				FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ChargingEffectClass, 1.f, Context);

				if (SpecHandle.IsValid())
				{
					ChargingEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

					UE_LOG(LogTemp, Warning, TEXT("ChargingEffect Applied"));
				}
			}
		}
		bHoldTriggered = true;
		GetWorld()->GetTimerManager().ClearTimer(HoldTH);

		UE_LOG(LogTemp, Warning, TEXT("UpdateHold : true"));
	}
}

void UGA_AttackCombo::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ChargingEffectHandle.IsValid())
	{
		// 핸들 초기화 보장
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(ChargingEffectHandle);
		UE_LOG(LogTemp, Warning, TEXT("ChargingEffect Removed"));
		ChargingEffectHandle = FActiveGameplayEffectHandle();
	}

	ClearComboWindowTasks();

	ComboIndex = 0;
	bCanCombo = false;
}

void UGA_AttackCombo::Server_ReleasedInput_Implementation()
{
	ReleasedInput();
}


