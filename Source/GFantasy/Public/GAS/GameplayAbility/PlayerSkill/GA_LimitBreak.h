// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GAS/GameplayAbility/PlayerSkill/GA_BaseAOE.h"
#include "GA_LimitBreak.generated.h"

/**
 * 
 */

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAnimMontage;
class APlayerAOEBase;
class ALimitBreakCamera;


UCLASS()
class GFANTASY_API UGA_LimitBreak : public UGA_BaseAOE
{
	GENERATED_BODY()
	
public:
	UGA_LimitBreak();

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

	// 1��° AOE ���� ����
	UFUNCTION()
	void SpawnSlashAOE(FGameplayEventData Payload);

	// 2��° AOE ���� ����
	UFUNCTION()
	void SpawnExplosionAOE(FGameplayEventData Payload);

	UFUNCTION()
	void OnFXNotifyReceived(FGameplayEventData Payload);

private:

	// AOE ���� ����
	void SpawnAOE(TSubclassOf<APlayerAOEBase> AOEClass);

	// ������ ���� �����ؼ� VFX ����
	void GetActorFeetZ(FVector& SpawnLocation);

	void SwitchViewTarget();
public:
	//������ ���� Explosion ��Ÿ��
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Attack")
	UAnimMontage* LimitBreakMontage;

	UPROPERTY(EditDefaultsOnly, Category="AOE")
	UNiagaraSystem* VisualOnlyFX;

	// ���� ���� ��Ÿ�ָ� �����ϴ� Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* AttackStartTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* AttackEndTask;

	// ����Ʈ �극��ũ �� �߰� AOE
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<APlayerAOEBase> ExplosionAOE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<ALimitBreakCamera> LimitCameraClass;
	
protected:

	UPROPERTY()
	APlayerAOEBase* CachedAOEActor;

	UPROPERTY()
	ALimitBreakCamera* SpawnedCamera;


private:

	
};
