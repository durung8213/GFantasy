// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "GASAbilityBase.generated.h"

USTRUCT(BlueprintType)
struct FComboSectionData
{
	GENERATED_BODY()

	// 스텝을 트리거할 GameplayCue 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	FGameplayTag ComboCueTag;

	// 몽타주 내 섹션 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	FName SectionName;

};

UCLASS(Abstract, BlueprintType)
class GFANTASY_API UGASAbilityBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AbilitySet")
	void GiveAbilitiesToASC(class UAbilitySystemComponent* ASC) const;
	//virtual void GiveAbilitiesToASC_Implementation(class UAbilitySystemComponent* ASC) const;
	
public:
	// 게임 어빌리티 클래스
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ActionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TArray<FComboSectionData> ComboSections;

};
