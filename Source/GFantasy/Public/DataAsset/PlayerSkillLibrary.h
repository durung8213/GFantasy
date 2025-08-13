// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "PlayerSkillLibrary.generated.h"

class UGA_Base;

/**
 * 
 */
UCLASS(BlueprintType)
class GFANTASY_API UPlayerSkillLibrary : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Skill Library")
	TSubclassOf<UGameplayAbility> GetAbilityClass(const FGameplayTag Tag) const;

	UFUNCTION(BlueprintPure, Category = "Skill Library")
	void GetAllSkillTags(TArray<FGameplayTag>& OutTags) const;

public:
	// 에디터에서 설정할 SkillTag -> AbilityClass
	UPROPERTY(EditAnywhere, Category = "Skill Library")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> SkillMap;
	

	
	
};
