// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/GameplayAbility/GA_ComboAttack.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimMontage.h"
#include "GAS/GamePlayTag.h"


void UGA_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Combo1Task();
}

void UGA_ComboAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	if (HasAuthority(&ActivationInfo))
	{
		// 서버에서 실행 
		OnInputAttack();	//몽타주 실행하고
	}
	else
	{
		// 클라에서 실행하면
		ActorInfo->AbilitySystemComponent->ServerSetInputPressed(Handle);	// 서버에 클라이언트에서 InputPressed가 발동되었으니 서버에서도 InputPressed를 해달라 요청
		OnInputAttack();	// 몽타주 실행
	}
}

void UGA_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	bCanCombo = false;	// 값 초기화
	ComboCount = 0;		// 콤보 카운트 초기화
}

void UGA_ComboAttack::Combo1Task()
{
	Combo1PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("Combo1Montage"),
		Combo1Montage,
		1.f
		);	// 콤보 몽타주1을 실행하는 타스크를 만들고

	Combo1PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_ComboAttack::OnMontageCompleted);	// complete을 함수에 바인딩 하고
	Combo1PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_ComboAttack::OnMontageCanceled);		// cancled을 함수에 바인딩 하고

	Combo1PlayMontageTask->ReadyForActivation(); // Montage1을 실행한다
	bCanCombo = false;

	// 콤보 가능 Wait 받음
	CanComboTrueEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTags::Get().PlayerAttack_Combo_CanNextComboTrue,
		nullptr,
		true,
		false
	);	// 노티파이에서 콤보가 가능한 타이밍이라고 이벤트를 받기 위한 타스크 이다	

	CanComboTrueEvent->EventReceived.AddDynamic(this, &UGA_ComboAttack::CanComboTrue);	// 이벤트 리시브 함수를 연결한 뒤 
	
	CanComboTrueEvent->ReadyForActivation();	// 콤보 타이밍 시작 타스크 실행

	// 콤보 불가능 Wait 받음
	CanComboFalseEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTags::Get().PlayerAttack_Combo_CanNextComboFalse,
		nullptr,
		true,
		false
	);	// 노티파이에서 콤보가 불가능하다고 이벤트를 받기 위한 타스크 이다		

	CanComboFalseEvent->EventReceived.AddDynamic(this, &UGA_ComboAttack::CanComboFalse);	// 이벤트 리시브 함수를 연결한 뒤 
	CanComboFalseEvent->ReadyForActivation();	// 콤보 타이밍 끝 타스크 실행
	
	ComboCount++;	// 콤보 카운트 증가
}

void UGA_ComboAttack::Combo2Task()
{
	Combo2PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("Combo1Montage"),
		Combo2Montage,
		1.f
		);	// 콤보 몽타주2을 실행하는 타스크를 만들고

	Combo2PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_ComboAttack::OnMontageCompleted);	// complete를 만든 뒤
	Combo2PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_ComboAttack::OnMontageCanceled);	// complete를 만든 뒤

	Combo2PlayMontageTask->ReadyForActivation(); // 실행한다
	bCanCombo = false;
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("몽타주 재생 Combo2Montage")));

	// 콤보 가능 Wait 받음
	CanComboTrueEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTags::Get().PlayerAttack_Combo_CanNextComboTrue,
		nullptr,
		true,
		false
	);	// Wait Task를 만들고 		

	CanComboTrueEvent->EventReceived.AddDynamic(this, &UGA_ComboAttack::CanComboTrue);	// 이벤트 리시브 함수를 연결한 뒤 
	
	CanComboTrueEvent->ReadyForActivation();	// 타스크 실행

	// 콤보 불가능 Wait 받음
	CanComboFalseEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTags::Get().PlayerAttack_Combo_CanNextComboFalse,
		nullptr,
		true,
		false
	);	// Wait Task를 만들고 		

	CanComboFalseEvent->EventReceived.AddDynamic(this, &UGA_ComboAttack::CanComboFalse);	// 이벤트 리시브 함수를 연결한 뒤 
	CanComboFalseEvent->ReadyForActivation();	// 타스크 실행
	
	ComboCount++;	// 콤보 카운트 증가
}

void UGA_ComboAttack::Combo3Task()
{
	Combo3PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this,
	TEXT("Combo1Montage"),
	Combo3Montage,
	1.f
	);	// 몽타주 실행 타스크를 만들고

	Combo3PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_ComboAttack::OnMontageCompleted);	// complete를 만든 뒤
	Combo3PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_ComboAttack::OnMontageCanceled);	// complete를 만든 뒤

	Combo3PlayMontageTask->ReadyForActivation(); // 실행한다
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("몽타주 재생 Combo3Montage")));
}

void UGA_ComboAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ComboAttack::OnMontageCanceled()
{
}

void UGA_ComboAttack::OnInputAttack()
{
	if (bCanCombo)
	{
		if (CanComboTrueEvent)
		{
			CanComboTrueEvent->EndTask();
			CanComboTrueEvent = nullptr;
		}
		else
		{
			CanComboFalseEvent->EndTask();
			CanComboFalseEvent = nullptr;
		}
		bCanCombo = false;		// 콤보 불가능하게 막음
		if (ComboCount == 1)
		{
			Combo2Task();		// 콤보가 1이면 이걸로
		}
		else if (ComboCount == 2)
		{
			Combo3Task();		// 콤보가 2면 이걸로
		}
	}
}

void UGA_ComboAttack::CanComboTrue(FGameplayEventData Payload)
{
	bCanCombo = true;
}

void UGA_ComboAttack::CanComboFalse(FGameplayEventData Payload)
{
	bCanCombo = false;
}
