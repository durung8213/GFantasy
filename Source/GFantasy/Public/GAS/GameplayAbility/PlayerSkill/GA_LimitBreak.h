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

	// 1번째 AOE 생성 시작
	UFUNCTION()
	void SpawnSlashAOE(FGameplayEventData Payload);

	// 2번째 AOE 생성 시작
	UFUNCTION()
	void SpawnExplosionAOE(FGameplayEventData Payload);

	UFUNCTION()
	void OnFXNotifyReceived(FGameplayEventData Payload);

private:

	// AOE 스폰 로직
	void SpawnAOE(TSubclassOf<APlayerAOEBase> AOEClass);

	// 지면의 높이 보정해서 VFX 실행
	void GetActorFeetZ(FVector& SpawnLocation);

	void SwitchViewTarget();
public:
	//에디터 세팅 Explosion 몽타주
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Attack")
	UAnimMontage* LimitBreakMontage;

	UPROPERTY(EditDefaultsOnly, Category="AOE")
	UNiagaraSystem* VisualOnlyFX;

	// 현재 공격 몽타주를 실행하는 Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* AttackStartTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* AttackEndTask;

	// 리미트 브레이크 용 추가 AOE
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
