// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayAbility/GA_EquipAnimation.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Character/GASPlayerCharacter.h"
#include "Character/AnimInstance/GASPlayerAnimInstance.h"
#include "Character/Weapon/WeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"


UGA_EquipAnimation::UGA_EquipAnimation()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.EquipAnimation")));
}

void UGA_EquipAnimation::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;

	if (Player->GetCombatState() == ECombatState::InCombat)
	{
		PlayEquipAnimation();
	}
	else
	{
		PlayUnEquipAnimation();
	}

}

void UGA_EquipAnimation::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (EquipTask)
	{
		EquipTask->EndTask();
		EquipTask = nullptr;
	}
	if (UnEquipTask)
	{
		UnEquipTask->EndTask();
		UnEquipTask = nullptr;
	}
	if (EquipEventTask)
	{
		EquipEventTask->EndTask();
		EquipEventTask = nullptr;
	}
	if (UnEquipEventTask)
	{
		UnEquipEventTask->EndTask();
		UnEquipEventTask = nullptr;
	}
}



void UGA_EquipAnimation::OnMontageCompleted()
{

	UE_LOG(LogTemp, Warning, TEXT("Equip Animation : ONMontageCompleted"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_EquipAnimation::OnMontageCancelled()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Equip Animation : OnMontageCancelled"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_EquipAnimation::EquipWeapon(FGameplayEventData Payload)
{
	if (AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (!WeaponClass)	return;

		AWeaponBase* Weapon = Player->GetCurrentWeapon();
		if (!Weapon)
		{
			UKismetSystemLibrary::PrintString(this, TEXT("No Current Weapon : Equip"));
			return;
		}
		// Weapon->AttachToComponent(Player->GetMesh(),
		// FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		// EquipSocket);
		//
		
		if (HasAuthority(&CurrentActivationInfo))
		{
			Weapon->Multicast_AttachWeapon(Player->GetMesh(), EquipSocket);
			UE_LOG(LogTemp, Warning, TEXT("Server: Multicast_AttachWeapon"));
		}
	}
}

void UGA_EquipAnimation::UnEquipWeapon(FGameplayEventData Payload)
{
	const AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Player)	return;
	
	if (!WeaponClass)
		return;


	AWeaponBase* Weapon = Player->GetCurrentWeapon();
	if (!Weapon)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("No Current Weapon : Equip"));
		return;
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		Weapon->Multicast_AttachWeapon(Player->GetMesh(), UnEquipSocket);
		UE_LOG(LogTemp, Warning, TEXT("Server: Multicast_AttachWeapon"));
	}
}

void UGA_EquipAnimation::PlayEquipAnimation()
{
	// 몽타주 실행.
	EquipTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	NAME_None,
	EquipMontage,
	1.0f,
	FName("UpperBody")  // Slot 이름
	);
	EquipTask->OnCompleted.AddDynamic(this, &UGA_EquipAnimation::OnMontageCompleted);
	EquipTask->OnCancelled.AddDynamic(this, &UGA_EquipAnimation::OnMontageCancelled);
	EquipTask->OnInterrupted.AddDynamic(this, &UGA_EquipAnimation::OnMontageCancelled);
	EquipTask->ReadyForActivation();

	// 노티파이 이벤트 기다리기.
	EquipEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Weapon.Equip"),
		nullptr,
		true,
		false
	);
	EquipEventTask->EventReceived.AddDynamic(this, &UGA_EquipAnimation::EquipWeapon);
	EquipEventTask->ReadyForActivation();
		
}

void UGA_EquipAnimation::PlayUnEquipAnimation()
{
	UnEquipTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	NAME_None,
	UnEquipMontage,
	1.0f,
	FName("UpperBody")  // Slot 이름
	);
	UnEquipTask->OnCompleted.AddDynamic(this, &UGA_EquipAnimation::OnMontageCompleted);
	UnEquipTask->OnCancelled.AddDynamic(this, &UGA_EquipAnimation::OnMontageCancelled);
	UnEquipTask->OnInterrupted.AddDynamic(this, &UGA_EquipAnimation::OnMontageCancelled);
	UnEquipTask->ReadyForActivation();

	
	// 노티파이 이벤트 기다리기.
	UnEquipEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag("Event.Weapon.UnEquip"),
		nullptr,
		true,
		false
	);
	UnEquipEventTask->EventReceived.AddDynamic(this, &UGA_EquipAnimation::UnEquipWeapon);
	UnEquipEventTask->ReadyForActivation();
}
