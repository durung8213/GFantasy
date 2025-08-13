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
	// 이벤트 리시버
	UFUNCTION()
	void OnTrailEvent(const FGameplayEventData Payload);

	UFUNCTION()
	void OnGroundEvent(const FGameplayEventData Payload);

	// 몽타주가 끝나면 Ability 종료
	UFUNCTION()
	void OnMontageCompleted();
	
	// AOE 생성 후, 실행할 함수
	void OnAOEActorSpawned(AFlameAttackAOE* SpawnedActor);

public:
	// 에디터 세팅 프로퍼티

	// 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* SmashMontage;

	/* 사용할 게임 태그 */

	UPROPERTY(EditDefaultsOnly, Category = "Events")
	FGameplayTag TrailEventTag;

	UPROPERTY(EditDefaultsOnly, Category = "Events")
	FGameplayTag GroundEventTag;
	
	//UPROPERTY(EditDefaultsOnly, Category = "VFX")
	//FGameplayTag CueTag_Trail;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FGameplayTag CueTag_Ground;

	// 검 끝 소켓 이름
	UPROPERTY(EditDefaultsOnly, Category = "AOE")
	FName WeaponSocketName = TEXT("FlameSword");

	// 바닥 VFX
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
