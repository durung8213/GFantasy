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

	// �����Ƽ
	// TryActivateAbility�� �ϰ� �Ǹ� �Ʒ��� �Լ��� ����ȴ�.
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

	// �Է� ��� Task
	void SetupComboWindow();

	// Task Clear
	void ClearComboWindowTasks();

	// ���� �½�ũ ����
	void SafeEndCurrentMontageTask();

	// ��Ÿ�� �Ϸ�
	UFUNCTION()
	void OnMontageCompleted();

	// ��Ÿ�� ���
	UFUNCTION()
	void OnMontageCanceled();

	// ù ��° �޺� ���� ���
	UFUNCTION()
	void PlayComboStep(int32 NewComboIndex);

	// 2�� ���� �޺� ���� ����
	void ComboAttack();

	// �̺�Ʈ �ڵ鷯 
	UFUNCTION()
	void CanComboTrue(FGameplayEventData Payload);

	UFUNCTION()
	void CanComboFalse(FGameplayEventData Payload);

	// Cue ����
	void ReleasedInput();
	
	UFUNCTION()
	void ExecuteChargeAbility();

private:
	void ResetRotation();


	/* ��¡ ��ų */

	// ���� Input�� �Է��� �Ǿ��ִ��� �ƴ��� Ȯ���ϴ� �Լ�
	bool IsInputStillPressed();

	// Ÿ�̸ӷ� ����� �Լ� / ����ؼ� Ȧ�尡 �ǰ� �ִ����� üũ
	void UpdateHold();

public:

	//������ ���� ��Ÿ��
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TArray<UAnimMontage*> ComboMontages;

	// GE Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge")
	TSubclassOf<UGameplayEffect> ChargingEffectClass;

	UPROPERTY()
	FActiveGameplayEffectHandle ChargingEffectHandle;

	// ������ ���� ��¡ �����Ƽ Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag ChargingCueTag;

	// �޺� �Է��� �޾Ҵ��� Ȯ���ϴ� ����
	bool bComboInputReceived = false;

	// ���� ��ǲ�� ���� �� �ִ� ��Ȳ����.
	bool bCanReceiveInput = false;

	// ���� �޺� ������ �� �� �ִ��� ������ 
	bool bCanCombo = false;

	// �޺� �Է� ����
	bool bInputBuffered = false;

	// ���� �޺� �ε���
	int32 ComboIndex = 0;

	// ���� �޺� ���� ��Ÿ�ָ� �����ϴ� Task
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* CurrentMontageTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CanComboTrueTask;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* CanComboFalseTask;


private:
	//���� ��Ÿ�� ����
	bool bIsMontageDone = false;

	//���� InputReleased ����
	bool bIsInputReleased = false;

	//�󸶳� ���������� ����
	float HeldTime = 0.f;

	// ���� �ð� �̻� ���ȴ��� �÷���
	bool bHoldTriggered = false;

	// GAS���� Ÿ�̸� ����� �ڵ�
	FTimerHandle HoldTH;

	// ���� �����Ƽ ������ ���� Ÿ�̸� �ڵ�
	FTimerHandle TempHandle;

	// �� �� �̻��̸� " �� ���� " ���� �Ǵ��� �� ����
	float HoldThreshold = 1.15f;	// X �� �̻��̸� ��¡���� ����.
};
