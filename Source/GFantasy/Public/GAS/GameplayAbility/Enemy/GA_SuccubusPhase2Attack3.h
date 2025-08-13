// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_EndDelegateBase.h"
#include "GA_SuccubusPhase2Attack3.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_SuccubusPhase2Attack3 : public UGA_EndDelegateBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	FVector GetAttackSpawnLocation(int Count);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BossAbility")
	float AttackDecalDistance = 700.f;
};
