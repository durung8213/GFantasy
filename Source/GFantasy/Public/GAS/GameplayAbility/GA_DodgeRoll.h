// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_DodgeRoll.generated.h"

/**
 *
 */

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GFANTASY_API UGA_DodgeRoll : public UGA_Base
{
	GENERATED_BODY()

public:
	UGA_DodgeRoll();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageCompleted();

	
	UFUNCTION()
	void OnMontageCancelled();

private:
	//UAnimMontage* GetMontageByDirection(ACharacter* Character, const FVector& InputDir);

	FName GetSectionNameByIndex(const int32 InputIndex);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* ForwardMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* BackwardMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* RightMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* LeftMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAnimMontage* DodgeMontage;

	//¹«Àû ÀÌÆåÆ®
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TSubclassOf<UGameplayEffect> InvincibleEffect;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	UAbilityTask_PlayMontageAndWait* MontageTask;


};
