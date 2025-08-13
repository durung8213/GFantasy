// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_MountCall.generated.h"

/**
 * 
 */
class UInputMappingContext;
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GFANTASY_API UGA_MountCall : public UGA_Base
{
	GENERATED_BODY()
	
public:
	UGA_MountCall();

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
	UAnimMontage* CallMontage;

	// 현재 몽타주를 실행하는 Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* CurrentMontageTask;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="IMC")
	UInputMappingContext* IMC_None;


};
