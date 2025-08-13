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
		
	// ī�޶� ȸ��
	PC->SetControlRotation(PlayerRotation);
	
	
	// 1. ī�޶� ���� ����
	SwitchViewTarget();
	
	// 2. ��ų ��Ÿ�� ����
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


	// ��Ƽ���� �˸��� �� ������ AOE Overlap �̺�Ʈ ����

	// VFX ���̾ư��� ���� ���
	UAbilityTask_WaitGameplayEvent* FXPlay = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.LimitBreak.FX")
	);
	FXPlay->EventReceived.AddDynamic(this, &UGA_LimitBreak::OnFXNotifyReceived);
	FXPlay->ReadyForActivation();

	// 3. 1��° AOE ���� Event ���( ���� ���� )
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

	// 4. 2��° AOE Event ���
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

		// ī�޶� ��ȯ
		PC->SetViewTargetWithBlend(GetAvatarActorFromActorInfo(), BlendDuration);

		// ���� �Ϸ� �� ī�޶� ����
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

	// ������ �ʱ�ȭ
	if (ResourceAS)
	{
		FGameplayAttribute LimitGaugeAttr = ResourceAS->GetLimitGageAttribute();

		// Effect �ο��� �� ���������� �����.
		ASC->SetNumericAttributeBase(LimitGaugeAttr, 0.0f);

		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Limit.Ready")))
		{
			// �±� ����
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
	// 1���� AOE ���� ����
	SpawnAOE(AOEActorClass);

	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

}

// 2��° ���� VFX Overlap ����
void UGA_LimitBreak::SpawnExplosionAOE(FGameplayEventData Payload)
{
	// 2��° AOE ����
	if (CachedAOEActor)
	{
		// ���� �ش� AOE�� �����ִٸ� ���� ����
		//CachedAOEActor->Destroy();
	}

	SpawnAOE(ExplosionAOE);
}

// ��¡ VFX �ٴڿ� ��ġ
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
	// ĳ���� �ڱ� �ڽ� ��ġ �Ʒ��� AOE ��ġ
	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	APlayerAOEBase* AOE = GetWorld()->SpawnActorDeferred<APlayerAOEBase>(
		AOEClass,
		FTransform(SpawnLocation),
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	// AOE ���� �� Ÿ�� ����.
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

	// Ŭ�󿡼��� ī�޶� ����
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

