// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_MountRide.generated.h"

/**
 * 
*/

class UAnimMonatage;
class UAbilityTask_PlayMontageAndWait;
class UInputMappingContext;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GFANTASY_API UGA_MountRide : public UGA_Base
{
	GENERATED_BODY()
	
	UGA_MountRide();

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

	// 이벤트 핸들러 
	UFUNCTION()
	void RideStart(FGameplayEventData Payload);
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montage")
	UAnimMontage* RideMontage;

	// 현재 몽타주를 실행하는 Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* CurrentMontageTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* PossessEventTask;


};
