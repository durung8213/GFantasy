// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/PlayerSkill/GA_BaseAOE.h"
#include "GA_RainAttack.generated.h"

/**
 * 
 */

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAnimMontage;
class APlayerAOEBase;

UCLASS()
class GFANTASY_API UGA_RainAttack : public UGA_BaseAOE
{
	GENERATED_BODY()

public:
	UGA_RainAttack();

	
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

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void SpawnAOE(FGameplayEventData Payload);
	
public:
	UPROPERTY(VisibleAnywhere)
	FVector CachedTargetLocation;

protected:

	// ��ų ��Ÿ��
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Attack")
	UAnimMontage* SkillMontage;
	
	// ���� ���� ��Ÿ�ָ� �����ϴ� Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* FirstAOETask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* SecondAOETask;

private:
	
	
	
	
};
