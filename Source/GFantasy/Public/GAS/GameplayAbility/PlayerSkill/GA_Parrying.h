// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_Parrying.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_Parrying : public UGA_Base
{
	GENERATED_BODY()

public:
	UGA_Parrying();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

public:
	// ¹Ù´Ú VFX
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UNiagaraSystem* ParryFX;

};
