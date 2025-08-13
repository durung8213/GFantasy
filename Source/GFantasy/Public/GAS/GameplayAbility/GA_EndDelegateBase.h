// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_EndDelegateBase.generated.h"

class AAIControllerBase;
class UAbilitySystemComponent;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndAbilityDelegate);

UCLASS()
class GFANTASY_API UGA_EndDelegateBase : public UGA_Base
{
	GENERATED_BODY()


public:
	// 어빌리티가 실행되면 실행
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// 어빌리티가 끝나면 실행
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AAIControllerBase* GetCurrentController();

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(UAbilitySystemComponent* Source, UAbilitySystemComponent* Target, TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABaseWeapon* GetAvatarActorWeapon();
public:
	FEndAbilityDelegate EndAbilityDelegate;
};
