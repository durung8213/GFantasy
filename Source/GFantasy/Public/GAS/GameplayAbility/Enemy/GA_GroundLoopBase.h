// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_EndDelegateBase.h"
#include "GA_GroundLoopBase.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_GroundLoopBase : public UGA_EndDelegateBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	void BindTargetHealth(UAbilitySystemComponent* TargetACS);
	
};
