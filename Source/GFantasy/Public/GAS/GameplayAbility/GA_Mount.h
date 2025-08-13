// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_Mount.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_Mount : public UGA_Base
{
	GENERATED_BODY()

protected:
	// 어빌리티가 실행되면 실행
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;

	
};
