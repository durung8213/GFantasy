// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/PlayerSkill/GA_BaseAOE.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_SwordFlameAttack.generated.h"


class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class AFlameAttackAOE;

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_SwordFlameAttack : public UGA_BaseAOE
{
	GENERATED_BODY()
	
public:
	UGA_SwordFlameAttack();

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
private:
	// �̺�Ʈ ���ù�
	UFUNCTION()
	void OnTrailEvent(const FGameplayEventData Payload);

	UFUNCTION()
	void OnGroundEvent(const FGameplayEventData Payload);

	// ��Ÿ�ְ� ������ Ability ����
	UFUNCTION()
	void OnMontageCompleted();
	
	// AOE ���� ��, ������ �Լ�
	void OnAOEActorSpawned(AFlameAttackAOE* SpawnedActor);

public:
	// ������ ���� ������Ƽ

	// �ִϸ��̼� ��Ÿ��
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* SmashMontage;

	/* ����� ���� �±� */

	UPROPERTY(EditDefaultsOnly, Category = "Events")
	FGameplayTag TrailEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Events")
	FGameplayTag GroundEventTag;
	
	//UPROPERTY(EditDefaultsOnly, Category = "VFX")
	//FGameplayTag CueTag_Trail;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FGameplayTag CueTag_Ground;

	// �� �� ���� �̸�
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	FName WeaponSocketName = TEXT("FlameSword");

	// �ٴ� VFX
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	class UNiagaraSystem* GroundNiagaraSystem;

	/* Gameplay Task */

	UPROPERTY(EditDefaultsOnly, Category = "SwordFlameAttack")
	UAbilityTask_PlayMontageAndWait* MontageTask;
	
	UPROPERTY(EditDefaultsOnly, Category = "SwordFlameAttack")
	UAbilityTask_WaitGameplayEvent* TrailTask;

	UPROPERTY(EditDefaultsOnly, Category = "SwordFlameAttack")
	UAbilityTask_WaitGameplayEvent* GroundTask;


	
};
