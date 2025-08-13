// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/PlayerSkill/GA_ChargeSkill.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/Weapon/FlameAttackAOE.h"
#include "DrawDebugHelpers.h" 
#include "Character/Weapon/PlayerAOEBase.h"
#include "Character/GASPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"


UGA_ChargeSkill::UGA_ChargeSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Skill.Sword.ChargeCommand")));

}

void UGA_ChargeSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat"));

	//몽타주 실행
	AttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ChargingMontage
	);
	AttackTask->OnCompleted.AddDynamic(this, &UGA_ChargeSkill::OnMontageCompleted);
	AttackTask->OnCancelled.AddDynamic(this, &UGA_ChargeSkill::OnMontageCanceled);
	AttackTask->OnInterrupted.AddDynamic(this, &UGA_ChargeSkill::OnMontageCanceled);
	AttackTask->ReadyForActivation();


	// AOE 생성 이벤트 대기.
	AOENotifyTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, AOESpawnTag, nullptr, true
	);
	AOENotifyTask->EventReceived.AddDynamic(this, &UGA_ChargeSkill::SpawnAOEActor);
	AOENotifyTask->ReadyForActivation();
}

void UGA_ChargeSkill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat"));

	UKismetSystemLibrary::PrintString(this, TEXT("End Ability : ChargeSkill"));
}

void UGA_ChargeSkill::SpawnAOEActor(const FGameplayEventData Payload)
{

	if (!HasAuthority(&CurrentActivationInfo))
		return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	AGASPlayerCharacter* Char = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)	return;

	// 캐릭터의 Weapon을 가져와서 Static Mesh의 소켓 위치를 가져와서 라인 트레이스
	const FVector SocketLocation = Char->GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart = SocketLocation + FVector(0.f,0.f,50.f);
	const FVector TraceEnd = TraceStart - FVector(0.f, 0.f, 1000.f);	//아래로 1000cm
	// 4) 라인 트레이스
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Char);

	bool bHit = Char->GetWorld()->LineTraceSingleByChannel(
		Hit, TraceStart, TraceEnd, ECC_Visibility, Params);

	DrawDebugLine(Char->GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 1.0f, 0, 2.0f);
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	if (bHit && AOEActorClass)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("bHit True"));

		// 클라이언트에서 카메라 연출 호출
		if (APlayerController* PC = Cast<APlayerController>(Char->GetController()))
		{
			PC->ClientStartCameraShake(CameraShakeClass);
		}

		// AOE 스폰
		SpawnAOEActorAtLocation(Hit);

	}
	else
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Flame : No Hit"));
	}
}

void UGA_ChargeSkill::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ChargeSkill::OnMontageCanceled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
