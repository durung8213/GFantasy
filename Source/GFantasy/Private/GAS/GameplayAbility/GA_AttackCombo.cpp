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
	// �ν��Ͻ� ��� ����
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//�ش� �����Ƽ�� �±� �ο�
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Combo")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Combo")));


}

void UGA_AttackCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKismetSystemLibrary::PrintString(this, TEXT("Attack Combo Start"));

	//���ҽ� �Һ� ��ٿ�
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Cool Down End"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	//�⺻ ���� �ʱ�ȭ
	ComboIndex = 0;
	bCanCombo = false;
	bIsMontageDone = false;
	bIsInputReleased = false;
	bHoldTriggered = false;
	HeldTime = 0.f;
	CurrentMontageTask = nullptr;

	// �޺� ������ �غ�
	SetupComboWindow();

	ResetRotation();

	/* ��¡ ���� */

	// �Է� �ð� �ʱ�ȭ
	HeldTime = 0.f;
	bHoldTriggered = false;
	bIsInputReleased = false;

	// 0.05�ʸ��� UpdateHold ����
	GetWorld()->GetTimerManager().SetTimer(
		HoldTH,
		this,
		&UGA_AttackCombo::UpdateHold,
		0.05f,
		true	//�ݺ�����
	);

	// ù ��° �޺� ���� ����
	PlayComboStep(ComboIndex);
}

// �����Ƽ������ �Է� �����Ƽ�� ������ ���, �Է��� ������ �Ʒ��� �Լ��� ����ȴ�.
void UGA_AttackCombo::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	// �޺� �����찡 �������� ������ ����.

	if (!bCanCombo )
	{
		return;
	}

	// ���� / Ŭ�� �޺� ����.
	if (HasAuthority(&ActivationInfo) || IsPredictingClient())
	{
		// �������� ���� 
		ComboAttack();	//��Ÿ�� �����ϰ�
		UE_LOG(LogTemp, Warning, TEXT("AttackCombo : 1"));
	}
	if (!HasAuthority(&ActivationInfo))
	{
		// Ŭ���� ���, ���� RPC �����ϱ�.
		ActorInfo->AbilitySystemComponent->ServerSetInputPressed(Handle);
		UE_LOG(LogTemp, Warning, TEXT("AttackCombo : 2"));
	}
}


// �޺� ���� ���� 
void UGA_AttackCombo::ComboAttack()
{
	if (!bCanCombo)
	{
		//���� �޺� ������ ������ ���°� �ƴ϶��
		return;
	}

	// ���� Task �ʱ�ȭ
	SafeEndCurrentMontageTask();

	// �޺� �ε��� ���� �� ���� �ʱ�ȭ.
	++ComboIndex;
	bCanCombo = false;
	bIsMontageDone = false;

	// ���� �޺� ������ �½�ũ ����
	ClearComboWindowTasks();

	// ���� ���� ������ ��, Ÿ�� �ٶ󺸰� ȸ��.
	RotateToLockOnTaret();

	// comboIndex�� ���� ���� ����
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
		// ��������Ʈ ���ε� ����
		CurrentMontageTask->OnCompleted.RemoveAll(this);
		CurrentMontageTask->OnCancelled.RemoveAll(this);
		CurrentMontageTask->OnInterrupted.RemoveAll(this);

		// �½�ũ ����
		CurrentMontageTask->EndTask();
		CurrentMontageTask = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("SafeEndCurrentMontageTask Success"));
	}
}

void UGA_AttackCombo::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	bIsInputReleased = true;

	// Ÿ�̸� �ߴ�
	GetWorld()->GetTimerManager().ClearTimer(HoldTH);

	UE_LOG(LogTemp, Warning, TEXT("InputReleased"));


	// Ȧ�尡 Ʈ���ŵǾ��ٸ� ��¡ ��ų ����


	if (bHoldTriggered)
	{
		// ���� �ð� �̻� ���������� -> Ŀ�ǵ� ��ų ����
		// �����Ƽ �±� �����ͼ� �ش� �����Ƽ ����

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

		if (ASC)
		{

			// Tag ��� Effect & Cue ����
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

		// ASC���� �����Ƽ �ѱ�� �ش� �Լ����� �����Ƽ ����
		FTimerDelegate Delegate;
		Delegate.BindUFunction(PlayerASC, FName("DeferredActivateAbilityByTag"), ChargeTag);

		GetWorld()->GetTimerManager().SetTimer(
			TempHandle, Delegate, 0.05f, false
		);
		//GetWorld()->GetTimerManager().SetTimerForNextTick(Delegate);
		
		return;
	}

	// ��¡ �̹ߵ� / ��Ÿ�ְ� �̹� ���� ���� -> ��� ����.
	if (bIsMontageDone && !bHoldTriggered)
	{
		// �ƴϸ� �׳� �Ϲ� ���� ����
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
	// ���� �����Ƽ �������� ĳ���� ��������
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (!Player)
		return;

	bIsMontageDone = true;

	// �Է��� �̹� ������ ������ ��� ����.
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
	// // �Է��� ������ ���µ� �ƴϵ� �����ϱ�.
	// EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}

void UGA_AttackCombo::OnMontageCanceled()
{
	//ResetRotation();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

// 2~4�� �޺� ��Ÿ�� ����
void UGA_AttackCombo::PlayComboStep(int32 NewComboIndex)
{

	// �迭 ������ ����� (������ ������ �̹� �Ѱ����� ) -> OnMontageCompleted�� ����
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

{	// ���� �����Ƽ �������� ĳ���� ��������
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
		// �ڵ� �ʱ�ȭ ����
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


