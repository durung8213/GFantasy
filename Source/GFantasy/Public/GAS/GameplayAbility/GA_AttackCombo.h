// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "InputActionValue.h"
#include "GA_AttackCombo.generated.h"


class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class GFANTASY_API UGA_AttackCombo : public UGA_Base
{
	GENERATED_BODY()

public:
	UGA_AttackCombo();

	// 어빌리티
	// TryActivateAbility를 하게 되면 아래의 함수가 실행된다.
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	UFUNCTION(Server, Reliable)
	void Server_ReleasedInput();

protected:

	// 입력 대기 Task
	void SetupComboWindow();

	// Task Clear
	void ClearComboWindowTasks();

	// 현재 태스크 종료
	void SafeEndCurrentMontageTask();

	// 몽타주 완료
	UFUNCTION()
	void OnMontageCompleted();

	// 몽타주 취소
	UFUNCTION()
	void OnMontageCanceled();

	// 첫 번째 콤보 스텝 재생
	UFUNCTION()
	void PlayComboStep(int32 NewComboIndex);

	// 2단 이후 콤보 어택 시작
	void ComboAttack();

	// 이벤트 핸들러 
	UFUNCTION()
	void CanComboTrue(FGameplayEventData Payload);

	UFUNCTION()
	void CanComboFalse(FGameplayEventData Payload);

	// Cue 제거
	void ReleasedInput();
	
	UFUNCTION()
	void ExecuteChargeAbility();

private:
	void ResetRotation();


	/* 차징 스킬 */

	// 현재 Input이 입력이 되어있는지 아닌지 확인하는 함수
	bool IsInputStillPressed();

	// 타이머로 실행될 함수 / 계속해서 홀드가 되고 있는지를 체크
	void UpdateHold();

public:

	//에디터 세팅 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TArray<UAnimMontage*> ComboMontages;

	// GE Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge")
	TSubclassOf<UGameplayEffect> ChargingEffectClass;

	UPROPERTY()
	FActiveGameplayEffectHandle ChargingEffectHandle;

	// 에디터 세팅 차징 어빌리티 Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag ChargingCueTag;

	// 콤보 입력을 받았는지 확인하는 변수
	bool bComboInputReceived = false;

	// 현재 인풋을 받을 수 있는 상황인지.
	bool bCanReceiveInput = false;

	// 현재 콤보 공격을 할 수 있는지 없는지 
	bool bCanCombo = false;

	// 콤보 입력 버퍼
	bool bInputBuffered = false;

	// 현재 콤보 인덱스
	int32 ComboIndex = 0;

	// 현재 콤보 공격 몽타주를 실행하는 Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* CurrentMontageTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CanComboTrueTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CanComboFalseTask;


private:
	//현재 몽타주 상태
	bool bIsMontageDone = false;

	//현재 InputReleased 상태
	bool bIsInputReleased = false;

	//얼마나 눌러졌는지 추적
	float HeldTime = 0.f;

	// 일정 시간 이상 눌렸는지 플래그
	bool bHoldTriggered = false;

	// GAS에서 타이머 실행용 핸들
	FTimerHandle HoldTH;

	// 다음 어빌리티 실행을 위한 타이머 핸들
	FTimerHandle TempHandle;

	// 몇 초 이상이면 " 꾹 누름 " 으로 판단할 지 기준
	float HoldThreshold = 1.15f;	// X 초 이상이면 차징으로 간주.
};
