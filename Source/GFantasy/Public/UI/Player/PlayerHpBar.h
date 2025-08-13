// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/AttributeSet.h"
#include "PlayerHpBar.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class UProgressBar;
class UGASCharacterAS;

UCLASS()
class GFANTASY_API UPlayerHpBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWithASC(UAbilitySystemComponent* InASC,
		UGASCharacterAS* InAS);

	virtual void NativeConstruct() override;
protected:

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(meta=(BindWidget))
	UProgressBar* HealthProgressBar;


private:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	TWeakObjectPtr<UGASCharacterAS> AS;
	float CachedMaxHealth = 100.f;
	
};
 