// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/PlayerSkill/GA_BaseAOE.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_ChargeSkill.generated.h"

/**
 * 
 */
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class APlayerAOEBase;
class UCameraShakeBase;

UCLASS()
class GFANTASY_API UGA_ChargeSkill : public UGA_BaseAOE
{
	GENERATED_BODY()
	
public:

	UGA_ChargeSkill();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	// AOE 积己
	UFUNCTION()
	void SpawnAOEActor(const FGameplayEventData Payload);

protected:

	// 根鸥林 肯丰
	UFUNCTION()
	void OnMontageCompleted();

	// 根鸥林 秒家
	UFUNCTION()
	void OnMontageCanceled();


public:
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* AttackTask = nullptr;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* AOENotifyTask = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UAnimMontage* ChargingMontage;


	UPROPERTY(EditDefaultsOnly, Category = "AOE Tag")
	FGameplayTag AOESpawnTag;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;
protected:

	// AOE 积己 家南 捞抚
	UPROPERTY(VisibleAnywhere)
	FName SocketName = FName("SwordSocket");
	
	bool bReleased = false;
	
	bool bShouldAttck = false;

	float ChargeTime = 0.f;
	
	float MaxChargeTime = 3.f;

	FTimerHandle ChargeTimerHandle;

};
