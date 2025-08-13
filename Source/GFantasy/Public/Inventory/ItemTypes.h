// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Crafting/CraftTypes.h"
#include "ItemTypes.generated.h"

class UEquipmentDefinition;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FEquipmentItemProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UEquipmentDefinition> EquipmentClass = nullptr;
	
};

USTRUCT(BlueprintType)
struct FConsumableProps
{
	GENERATED_BODY()

	/*아이템 효과를 위한 Effect Class*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ItemEffectClass = nullptr;

	/*Effect Class의 성능 레벨*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ItemEffectLevel = 1.f;
};

USTRUCT(BlueprintType)
struct FCraftingProps
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FCraftRecipeItemGroup> RecipeItems = TArray<FCraftRecipeItemGroup>();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CraftedTag = FGameplayTag();
};

USTRUCT(BlueprintType)
struct FMaterialProps
{
	GENERATED_BODY()
	
};


USTRUCT(BlueprintType)
struct FMasterItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CategoryTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemDescription = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag RarityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	int32 ItemQuantity = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemPrice = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FConsumableProps ConsumableProps = FConsumableProps();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEquipmentItemProps EquipmentItemProps = FEquipmentItemProps();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FCraftingProps CraftingProps = FCraftingProps();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FMaterialProps IngredientProps = FMaterialProps();

};
