// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CraftTypes.generated.h"

USTRUCT()
struct FCraftRecipeItemGroup : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer RecipeItemTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AmountNeeded = 0;	
};