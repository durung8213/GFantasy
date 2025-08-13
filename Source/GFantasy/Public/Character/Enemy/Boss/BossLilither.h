// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCombatBossNPC.h"
#include "BossLilither.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API ABossLilither : public AGASCombatBossNPC
{
	GENERATED_BODY()
public:
	virtual void OnNormalAttackDie(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target) override;
	virtual void OnStrongAttackDie(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target) override;

	UFUNCTION(BlueprintCallable)
	void SetIsDead(bool NewbIsDead);

	UFUNCTION(BlueprintCallable)
	bool GetIsDead();
};
