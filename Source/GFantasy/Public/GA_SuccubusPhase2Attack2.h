// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_EndDelegateBase.h"
#include "GA_SuccubusPhase2Attack2.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_SuccubusPhase2Attack2 : public UGA_EndDelegateBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetSickleAuraSpawnLocation(float Distance);
};
