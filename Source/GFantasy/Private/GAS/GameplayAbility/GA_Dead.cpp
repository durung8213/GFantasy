// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_Dead.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GameFramework/Controller.h"
#include "GameState/GASWorldGameState.h"
#include "Character/GASPlayerCharacter.h"


UGA_Dead::UGA_Dead()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag("Ability.Dead"));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead.Player"));

}

void UGA_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 죽음 상태 태그 추가
	ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Dead.Player"));
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());

	Player->bIsDead = true;

	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 5.0f);
	DelayTask->OnFinish.AddDynamic(this, &UGA_Dead::OnDeathDelayComplete);
	DelayTask->Activate();

}

void UGA_Dead::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_Dead::OnDeathDelayComplete()
{
	if (!HasAuthority(&CurrentActivationInfo))
		return;

	if (AController* PC = Cast<AController>(GetActorInfo().PlayerController.Get()))
	{
		if (AGASWorldGameState* GS = GetWorld()->GetGameState<AGASWorldGameState>())
		{
			GS->RequestRespawn(PC);
	
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}
