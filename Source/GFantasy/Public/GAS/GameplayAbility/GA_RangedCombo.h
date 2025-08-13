// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbility.h"
#include "Character/Weapon/RangedProjectile.h"
#include "GA_RangedCombo.generated.h"

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;

/**
 * 
 */

class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GFANTASY_API UGA_RangedCombo : public UGA_Base
{
	GENERATED_BODY()
	
public:
	UGA_RangedCombo();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	void RangedAttackCombo();

	UFUNCTION()
	void OnMontageCanceled();

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnShootEventReceived(FGameplayEventData Payload);



public:
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* ShootEventTask;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ARangedProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	FName MuzzleSocket = "LeftHand_Ranged";

	// Option: ½´ÆÃ Montage
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* RangedAttackMontage;

};
