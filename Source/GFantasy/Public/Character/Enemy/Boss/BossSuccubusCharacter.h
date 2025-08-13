// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCombatBossNPC.h"
#include "BossSuccubusCharacter.generated.h"

/**
 * 
 */

class UCurveFloat;
class UTimelineComponent;

UCLASS()
class GFANTASY_API ABossSuccubusCharacter : public AGASCombatBossNPC
{
	GENERATED_BODY()
public:
	virtual void OnNormalAttackDie(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target) override;
	
	virtual void OnStrongAttackDie(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target) override;

	UFUNCTION(BlueprintCallable)
	void SetIsDead(bool NewbIsDead);

	UFUNCTION(BlueprintCallable)
	bool GetIsDead();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void ChangeCharacterAnimInstance(TSubclassOf<UAnimInstance> NewAnimInstance);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void ChangeCharacterAnimPhase(int Phase);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MultiChangeMeshTwoPhase();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeMeshTwoPhase();

	void TimelineStart(UCurveFloat* TimeLineCurve);

	void TimelineInProgress(float Value);

	void TimelineComplete();
	
protected:

	
};
