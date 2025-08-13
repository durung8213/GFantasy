// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "SkillSlotWidget.generated.h"

/**
 * 
 */

struct FGameplayTagContainer;
class UAbilitySystemComponent;

UCLASS()
class GFANTASY_API USkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void BindToASC(UPlayerCharacterGASC* ASC);

public:
	// 리미트 브레이크 UI
	UPROPERTY(meta = (BindWidget))
	class ULimitBreakWidget* WBP_LimitBreakSlot;

	// 차징 스킬 UI
	UPROPERTY(meta = (BindWidget))
	class UChargingSkillWidget* WBP_ChargeSkillSlot;

	// Q 스킬 UI
	UPROPERTY(meta = (BindWidget))
	class UQSkillSlotWidget* WBP_QSkillSlot;

	
};
