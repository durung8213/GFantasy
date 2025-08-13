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

	// 어빌리티의 DataAsset을 참조한다.
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
	
	// 어빌리티를 담을 배열
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<FAbilitySetItem> Abilities;



	virtual void GiveAbilitiesToASC_Implementation(class UAbilitySystemComponent* ASC) const override;
	
	
};
