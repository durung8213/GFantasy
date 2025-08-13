// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_UnMount.generated.h"

/**
 * 
 */
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GFANTASY_API UGA_UnMount : public UGA_Base
{
	GENERATED_BODY()
	
	UGA_UnMount();

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

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montage")
	UAnimMontage* UnMountMontage;
	
	// 현재 몽타주를 실행하는 Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* UnMountMontageTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* PossessEventTask;
};
