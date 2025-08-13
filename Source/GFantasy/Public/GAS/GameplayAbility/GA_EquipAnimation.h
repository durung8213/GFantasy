// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_EquipAnimation.generated.h"

/**
 * 
 */

class UAnimMontage;
class AWeaponBase;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GFANTASY_API UGA_EquipAnimation : public UGA_Base
{
	GENERATED_BODY()

public:

	UGA_EquipAnimation();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;


	
	
protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void EquipWeapon(FGameplayEventData Payload);

	UFUNCTION()
	void UnEquipWeapon(FGameplayEventData Payload);

	
	
private:
	void PlayEquipAnimation();

	void PlayUnEquipAnimation();
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montage")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montage")
	UAnimMontage* UnEquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Socket")
	FName EquipSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Socket")
	FName UnEquipSocket;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;


	// Task °ü¸®
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UAbilityTask_PlayMontageAndWait* EquipTask;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UAbilityTask_PlayMontageAndWait* UnEquipTask;

	UPROPERTY(EditDefaultsOnly)
	UAbilityTask_WaitGameplayEvent* EquipEventTask;
	
	UPROPERTY(EditDefaultsOnly)
	UAbilityTask_WaitGameplayEvent* UnEquipEventTask;
};
