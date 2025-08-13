// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayAbility/GA_GetUp.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GASPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


UGA_GetUp::UGA_GetUp()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.GetUp")));
}

void UGA_GetUp::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	// 루트모션 사용 중이면 이동 잠금
	if (Player)
	{
		Player->GetCharacterMovement()->DisableMovement();
	}

	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,NAME_None,Montage,1.f);
	MontageTask->OnCompleted.AddDynamic(this,&UGA_GetUp::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this,&UGA_GetUp::OnMontageCancelled);
	MontageTask->ReadyForActivation();

	
}

void UGA_GetUp::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	

}

void UGA_GetUp::OnMontageCompleted()
{
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	// 루트모션 사용 중이면 이동 잠금 해제
	if (Player)
	{
		Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_GetUp::OnMontageCancelled()
{
	const AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	// 루트모션 사용 중이면 이동 잠금 해제
	if (Player)
	{
		Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
