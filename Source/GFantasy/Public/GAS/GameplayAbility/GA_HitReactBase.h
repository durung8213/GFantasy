// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_HitReactBase.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_HitReactBase : public UGA_Base
{
	GENERATED_BODY()
	

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageEnded();

public:

	// Hit �׼� ��Ÿ��
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	TSoftObjectPtr <UAnimMontage> HitReactMontage;

	// ��Ÿ�� ���� �̸�
	UPROPERTY(EditDefaultsOnly, Category = "HitReact")
	FName SectionName;
	
	
};
