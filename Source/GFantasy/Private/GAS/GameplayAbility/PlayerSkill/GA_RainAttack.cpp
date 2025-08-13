// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/PlayerSkill/GA_RainAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GASPlayerCharacter.h"

UGA_RainAttack::UGA_RainAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Ability.Skill.Sword.RainAttack"));
	
}

void UGA_RainAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)	return;

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	if (Player->CurrentTarget)
	{
		// 타겟이 되는 적의 위치를 캐싱 ( 해당 위치에 스킬 시전 )
		CachedTargetLocation = Player->CurrentTarget->GetActorLocation();
	}
	else
	{
		// 타겟이 없을 경우 스킬 실행 종료.
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 스킬 몽타주 실행
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		SkillMontage,
		1.0f,
		NAME_None,
		false);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_RainAttack::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_RainAttack::OnMontageCancelled);
	MontageTask->ReadyForActivation();
	
	// 1번째 AOE 실행
	FirstAOETask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Ability.Skill.RainAttack.First"),
		nullptr,
		true,
		false);
	FirstAOETask->EventReceived.AddDynamic(
		this, &UGA_RainAttack::SpawnAOE);
	FirstAOETask->ReadyForActivation();

	// 2번째 AOE 실행
	SecondAOETask= UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Ability.Skill.RainAttack.Second"),
		nullptr,
		true,
		false);
	SecondAOETask->EventReceived.AddDynamic(
		this, &UGA_RainAttack::SpawnAOE);
	SecondAOETask->ReadyForActivation();
	
}

void UGA_RainAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_RainAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_RainAttack::OnMontageCancelled()
{	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);

}

// AOE 스폰 함수
void UGA_RainAttack::SpawnAOE(FGameplayEventData Payload)
{
	// 라인 트레이스로 지면에 
	FHitResult Hit;
	
	FVector TraceStart = CachedTargetLocation + FVector(0,0,0.0f);
	FVector TraceEnd = CachedTargetLocation - FVector(0,0,500.0f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
	
	
	// 라인 트레이스로 Current Target 위치에 AOE 스폰
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd,ECC_Visibility, Params))
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Yellow, false, 3.0f, 0, 2.0f);

		SpawnAOEActorAtLocation(Hit.ImpactPoint);
	}
	else
	{
		// 라인 트레이스 실패 시, 캐싱된 위치에 AOE 스폰
		SpawnAOEActorAtLocation(CachedTargetLocation);
	}
}
