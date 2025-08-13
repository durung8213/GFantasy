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
	// ����Ʈ �극��ũ UI
	UPROPERTY(meta = (BindWidget))
	class ULimitBreakWidget* WBP_LimitBreakSlot;

	// ��¡ ��ų UI
	UPROPERTY(meta = (BindWidget))
	class UChargingSkillWidget* WBP_ChargeSkillSlot;

	// Q ��ų UI
	UPROPERTY(meta = (BindWidget))
	class UQSkillSlotWidget* WBP_QSkillSlot;

	
};
