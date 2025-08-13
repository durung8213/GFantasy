// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/GameplayAbility/GA_UnMount.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actor/Riding/RidingComponent.h"
#include "Character/GASPlayerCharacter.h"


UGA_UnMount::UGA_UnMount()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Mount.UnMount")));
}

void UGA_UnMount::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		return;

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)
		return;

	// ������ ������ ��Ÿ�� ����
	UnMountMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this, NAME_None, UnMountMontage,1.f);
	UnMountMontageTask->OnCompleted.AddDynamic(this, &UGA_UnMount::OnMontageCompleted);
	UnMountMontageTask->ReadyForActivation();
}

void UGA_UnMount::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_UnMount::OnMontageCompleted()
{
	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	if (URidingComponent* RC = Player->GetRidingComponent())
	{
		if (HasAuthority(&CurrentActivationInfo))
		{
			// ���̵� ������Ʈ���� UnPossess, Attach ����
			RC->UnMount();
		}
	}
	
	// �����Ƽ ����
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}
