// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA_Base.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_Base : public UGameplayAbility
{
	GENERATED_BODY()
	
	
public:
	UGA_Base();

	//FGameplayTagContainer GetActivationOwnedTags();
	
	void RotateToLockOnTaret();

};
