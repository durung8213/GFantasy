// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AbilitySystemComponent/GASCBase.h"
#include "CombatNonPlayerGASC.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetsTag, const FGameplayTagContainer&)

UCLASS()
class GFANTASY_API UCombatNonPlayerGASC : public UGASCBase
{
	GENERATED_BODY()
	
public:
	void AbilityActorInfoSet();

	UFUNCTION(BlueprintCallable)
	void ActiveAbilityByTag(const FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	bool GetAbilityIsActiveByTag(const FGameplayTag Tag);
	
protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

public:
	FEffectAssetsTag EffectAssetsTag;
};
