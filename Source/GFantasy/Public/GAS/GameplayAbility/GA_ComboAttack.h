// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_ComboAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_ComboAttack : public UGA_Base
{
	GENERATED_BODY()
	
	// 어빌리티가 실행되면 실행
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 플레이어가 입력을 누르면 실행
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// 어빌리티가 끝나면 실행
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// 콤보 1을 실행시키는 타스크 생성
	UFUNCTION()
	void Combo1Task();

	// 콤보 2을 실행시키는 타스크 생성
	UFUNCTION()
	void Combo2Task();

	// 콤보 3을 실행시키는 타스크 생성
	UFUNCTION()
	void Combo3Task();

	// 몽타주가 성공적으로 끝났을 때 실행하는 바인딩용 함수
	UFUNCTION()
	void OnMontageCompleted();

	// 몽타주가 캔슬 되었을 때 실행하는 바인딩용 함수
	UFUNCTION()
	void OnMontageCanceled();

	// 입력이 들어왔을 때 진짜로 처리할 부분
	UFUNCTION()
	void OnInputAttack();

	// 콤보가 가능하게 설정하는 함수
	UFUNCTION()
	void CanComboTrue(FGameplayEventData Payload);

	// 콤보가 불가능하게 설정하는 함수
	UFUNCTION()
	void CanComboFalse(FGameplayEventData Payload);
	
public:
	// 콤보공격 1 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Combo1Montage;

	// 콤보공격 2 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Combo2Montage;

	// 콤보공격 3 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Combo3Montage;

protected:
	// 콤보가 가능한지
	bool bCanCombo = false;

	// 현재 콤보 카운트
	int ComboCount = 0;

	// 콤보 몽타주 타스크
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* Combo1PlayMontageTask;
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* Combo2PlayMontageTask;
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* Combo3PlayMontageTask;

	// 콤보 가능 불가능 Evnet 수신 타스크
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CanComboTrueEvent;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CanComboFalseEvent;
	
};
