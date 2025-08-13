// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/GASAbilityBase.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "GFantasyEnums.h"
#include "GASAbilitySet.generated.h"

USTRUCT(BlueprintType)
struct FAbilitySetItem {
	GENERATED_BODY()

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UGameplayAbility> Ability;*/

	// �����Ƽ�� DataAsset�� �����Ѵ�.
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InputIndex")
	EAbilityInputID InputID = EAbilityInputID::None;
};

UCLASS(BlueprintType)
class GFANTASY_API UGASAbilitySet : public UGASAbilityBase
{
	GENERATED_BODY()
	
public:
	
	// �����Ƽ�� ���� �迭
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<FAbilitySetItem> Abilities;



	virtual void GiveAbilitiesToASC_Implementation(class UAbilitySystemComponent* ASC) const override;
	
	
};
