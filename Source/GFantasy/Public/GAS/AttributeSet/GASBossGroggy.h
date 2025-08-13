// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GASBossGroggy.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGroggyChargeMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGroggyUseUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExtraGaugeFull);

UCLASS()
class GFANTASY_API UGASBossGroggy : public UAttributeSet
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	virtual void OnRep_Groggy(const FGameplayAttributeData& OldGroggy);

	UFUNCTION()
	virtual void OnRep_MaxGroggy(const FGameplayAttributeData& OldMaxGroggy);

	UFUNCTION()
	virtual void OnRep_ExtraGauge(const FGameplayAttributeData& OldExtraGauge);

	UFUNCTION()
	virtual void OnRep_MaxExtraGauge(const FGameplayAttributeData& OldMaxExtraGauge);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_Groggy)
	FGameplayAttributeData Groggy;
	ATTRIBUTE_ACCESSORS(UGASBossGroggy, Groggy)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_MaxGroggy)
	FGameplayAttributeData MaxGroggy;
	ATTRIBUTE_ACCESSORS(UGASBossGroggy, MaxGroggy)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_ExtraGauge)
	FGameplayAttributeData ExtraGauge;
	ATTRIBUTE_ACCESSORS(UGASBossGroggy, ExtraGauge)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", ReplicatedUsing = OnRep_MaxExtraGauge)
	FGameplayAttributeData MaxExtraGauge;
	ATTRIBUTE_ACCESSORS(UGASBossGroggy, MaxExtraGauge)
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	// 현재 수치가 최대 수치를 넘지 못하도록 Clamp 하는 함수
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

public:
	UPROPERTY()
	FOnGroggyChargeMax OnGroggyChargeMax;

	UPROPERTY()
	FOnGroggyUseUp OnGroggyUseUp;

	UPROPERTY()
	FOnExtraGaugeFull OnExtraGaugeFull;
};
