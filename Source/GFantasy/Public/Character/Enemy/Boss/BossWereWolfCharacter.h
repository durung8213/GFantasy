// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCombatBossNPC.h"
#include "Character/GASCombatNPC.h"
#include "BossWereWolfCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API ABossWereWolfCharacter : public AGASCombatBossNPC
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MultiRotateActor(AActor* Target, FRotator TargetRotator);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MultiSetLocationActor(AActor* Target, FVector TargetVector);
	
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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Phase2AnimInstance;
	
};
