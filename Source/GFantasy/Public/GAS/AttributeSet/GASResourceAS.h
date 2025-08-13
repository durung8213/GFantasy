// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "GameplayAbilities/Public/AttributeSet.h"
#include "GASResourceAS.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GFANTASY_API UGASResourceAS : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void OnRep_LimitGage(const FGameplayAttributeData& OldLimitGage);

	UFUNCTION()
	virtual void OnRep_MaxLimitGage(const FGameplayAttributeData& OldMaxLimitGage);


public:
	// 부스트 게이지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_LimitGage)
	FGameplayAttributeData LimitGage;
	ATTRIBUTE_ACCESSORS(UGASResourceAS, LimitGage)

	// Max 부스트 게이지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_MaxLimitGage)
	FGameplayAttributeData MaxLimitGage;
	ATTRIBUTE_ACCESSORS(UGASResourceAS, MaxLimitGage)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	
	
};
