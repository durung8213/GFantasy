// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayAbility/GA_MountRide.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actor/Riding/RidingComponent.h"
#include "Character/GASPlayerCharacter.h"


UGA_MountRide::UGA_MountRide()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Mount.Ride")));
}

void UGA_MountRide::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	
	// 아무것도 실행할 수 없는 IMC 적용
	//Player->ApplyIMC(IMC_None,0);
			
	CurrentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, RideMontage,1.f);
	CurrentMontageTask->OnCompleted.AddDynamic(this, &UGA_MountRide::OnMontageCompleted);
	CurrentMontageTask->OnCancelled.AddDynamic(this, &UGA_MountRide::OnMontageCompleted);
	CurrentMontageTask->ReadyForActivation();

	PossessEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Mount.Ride.Start"),
		nullptr,
		true,
		false
	);
	PossessEventTask->EventReceived.AddDynamic(this, &UGA_MountRide::RideStart);
	PossessEventTask->ReadyForActivation();

	
}

void UGA_MountRide::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (PossessEventTask)
		PossessEventTask->EndTask();
	
	if (CurrentMontageTask)
		CurrentMontageTask->EndTask();
}

void UGA_MountRide::OnMontageCompleted()
{
	if (PossessEventTask)
		PossessEventTask->EndTask();
	
	if (CurrentMontageTask)
		CurrentMontageTask->EndTask();
	// 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}

void UGA_MountRide::RideStart(FGameplayEventData Payload)
{
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	if (URidingComponent* RC = Player->GetRidingComponent())
	{
		// Attach, Possess 실행.
		if (HasAuthority(&CurrentActivationInfo))
		{
			RC->Mount();
		}
	}
}
