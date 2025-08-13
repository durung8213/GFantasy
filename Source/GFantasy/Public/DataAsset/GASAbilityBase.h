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

	// ������ Ʈ������ GameplayCue �±�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	FGameplayTag ComboCueTag;

	// ��Ÿ�� �� ���� �̸�
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
	// ���� �����Ƽ Ŭ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ActionMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TArray<FComboSectionData> ComboSections;

};
