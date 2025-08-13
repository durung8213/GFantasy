// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/PlayerSkill/GA_LimitBreak.h"
#include "GAS/AttributeSet/GASResourceAS.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/Weapon/PlayerAOEBase.h"
#include "NiagaraFunctionLibrary.h"
#include "Actor/Camera/LimitBreakCamera.h"
#include "Character/GASPlayerCharacter.h"

UGA_LimitBreak::UGA_LimitBreak()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Skill.LimitBreak")));
	//ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Limit.Ready")));

}

void UGA_LimitBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UKismetSystemLibrary::PrintString(this, TEXT("UGA_LimitBreak"));
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)	return;

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	if (!PC)	return;

	FRotator PlayerRotation = Player->GetActorRotation();
		
	// 카메라 회전
	PC->SetControlRotation(PlayerRotation);
	
	
	// 1. 카메라 연출 시작
	SwitchViewTarget();
	
	// 2. 스킬 몽타주 실행
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		LimitBreakMontage,
		1.0f,
		NAME_None,
		false
	);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_LimitBreak::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_LimitBreak::OnMontageCancelled);
	MontageTask->ReadyForActivation();

	UKismetSystemLibrary::PrintString(this, TEXT("End Ability : UGA_LimitBreak"));


	// 노티파이 알림이 올 때마다 AOE Overlap 이벤트 실행

	// VFX 나이아가라 실행 대기
	UAbilityTask_WaitGameplayEvent* FXPlay = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.LimitBreak.FX")
	);
	FXPlay->EventReceived.AddDynamic(this, &UGA_LimitBreak::OnFXNotifyReceived);
	FXPlay->ReadyForActivation();

	// 3. 1번째 AOE 스폰 Event 대기( 공격 시작 )
	AttackStartTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.LimitBreak.SlashWindow.Start"),
		nullptr,
		true,
		false
	);
	AttackStartTask->EventReceived.AddDynamic(this,
		&UGA_LimitBreak::SpawnSlashAOE);
	AttackStartTask->ReadyForActivation();

	// 4. 2번째 AOE Event 대기
	AttackEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.LimitBreak.ExplosionWindow.Start"),
		nullptr,
		true,
		false
	);
	AttackEndTask->EventReceived.AddDynamic(this,
		&UGA_LimitBreak::SpawnExplosionAOE);
	AttackEndTask->ReadyForActivation();

}

void UGA_LimitBreak::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)	return;

	APlayerController* PC = Cast<APlayerController>(ActorInfo->PlayerController);
	if (PC)
	{
		const float BlendDuration = 1.2f;

		// 카메라 전환
		PC->SetViewTargetWithBlend(GetAvatarActorFromActorInfo(), BlendDuration);

		// 블렌드 완료 후 카메라 제거
		FTimerHandle DestroyCamTimer;
		GetWorld()->GetTimerManager().SetTimer(DestroyCamTimer, [this]()
		{
			if (IsValid(SpawnedCamera))
			{
				SpawnedCamera->Destroy();
			}
		}, BlendDuration, false);
	}

	const UGASResourceAS* ResourceAS = Cast<UGASResourceAS>(ASC->GetAttributeSet(
	UGASResourceAS::StaticClass()));

	// 게이지 초기화
	if (ResourceAS)
	{
		FGameplayAttribute LimitGaugeAttr = ResourceAS->GetLimitGageAttribute();

		// Effect 부여로 쭉 내려가도록 만들기.
		ASC->SetNumericAttributeBase(LimitGaugeAttr, 0.0f);

		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Limit.Ready")))
		{
			// 태그 제거
			ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Limit.Ready"));
			UE_LOG(LogTemp, Warning, TEXT("Tag Removed Manually: State.Limit.Ready"));
		}
	}
}

void UGA_LimitBreak::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_LimitBreak::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_LimitBreak::SpawnSlashAOE(FGameplayEventData Payload)
{
	// 1번쨰 AOE 공격 실행
	SpawnAOE(AOEActorClass);

	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

}

// 2번째 폭발 VFX Overlap 생성
void UGA_LimitBreak::SpawnExplosionAOE(FGameplayEventData Payload)
{
	// 2번째 AOE 생성
	if (CachedAOEActor)
	{
		// 아직 해당 AOE가 남아있다면 강제 삭제
		//CachedAOEActor->Destroy();
	}

	SpawnAOE(ExplosionAOE);
}

// 차징 VFX 바닥에 설치
void UGA_LimitBreak::OnFXNotifyReceived(FGameplayEventData Payload)
{
	UWorld* World = GetWorld();
	if (!World || !VisualOnlyFX)
		return;

	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	GetActorFeetZ(SpawnLocation);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		VisualOnlyFX,
		SpawnLocation,
		FRotator::ZeroRotator,
		FVector(1.f),
		true,
		true,
		ENCPoolMethod::AutoRelease
	);

}

void UGA_LimitBreak::SpawnAOE(TSubclassOf<APlayerAOEBase> AOEClass)
{	
	// 캐릭터 자기 자신 위치 아래에 AOE 설치
	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	APlayerAOEBase* AOE = GetWorld()->SpawnActorDeferred<APlayerAOEBase>(
		AOEClass,
		FTransform(SpawnLocation),
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	// AOE 생성 및 타격 시작.
	AOE->Initialize(GetAbilitySystemComponentFromActorInfo(),
		AOE->DamageGE,
		AOE->DamageRadius,
		AOE->NiagaraFX,
		AOE->TotalDamageCount,
		AOE->DamageInterval);

	AOE->FinishSpawning(FTransform(SpawnLocation));
	CachedAOEActor = AOE;

}

void UGA_LimitBreak::GetActorFeetZ(FVector& SpawnLocation)
{
	FHitResult HitResult;


	FVector TraceStart = SpawnLocation + FVector(0, 0, 50.0f);
	FVector TraceEnd = SpawnLocation - FVector(0, 0, 500.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		SpawnLocation = HitResult.Location;
	}

}

void UGA_LimitBreak::SwitchViewTarget()
{
	UWorld* World = GetWorld();

	// 클라에서만 카메라 생성
	if (World && LimitCameraClass && IsLocallyControlled())
	{
		FVector SpawnLoc = GetAvatarActorFromActorInfo()->GetActorLocation();
		SpawnedCamera = World->SpawnActor<ALimitBreakCamera>(
			LimitCameraClass,
			SpawnLoc,
			FRotator::ZeroRotator);

		if (SpawnedCamera)
		{
			SpawnedCamera->SetTargetActor(GetAvatarActorFromActorInfo());

			if (APlayerController* PC = Cast<APlayerController>(CurrentActorInfo->PlayerController))
			{
				PC->SetViewTargetWithBlend(SpawnedCamera, 0.5f);
			}

			SpawnedCamera->StartCameraSequence();
		}
	}
}

