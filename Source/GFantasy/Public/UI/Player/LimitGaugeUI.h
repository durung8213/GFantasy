// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "LimitGaugeUI.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UProgressBar;
class UGASResourceAS;

UCLASS()
class GFANTASY_API ULimitGaugeUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeWithASC(UAbilitySystemComponent* InASC,
		UGASResourceAS* InAS);

	virtual void NativeConstruct() override;

protected:

	void OnLimitGaugeChanged(const FOnAttributeChangeData& Data);



public:
	// limit È¿°úÀ½
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	USoundBase* LimitBreakReadySound;

protected:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* LimitGaugeBar;

private:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	TWeakObjectPtr<UGASResourceAS> AS;

	float CachedMaxLimitGauge = 100.0f;
	
	
};
