// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentDefinition.generated.h"

class AEquipmentActor;
class UEquipmentInstance;

USTRUCT()
struct FEquipmentActorsToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AEquipmentActor> EquipmentClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FName AttachName = FName();
	
};
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GFANTASY_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Info")
	FGameplayTag ItemTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Info")
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Info")
	FGameplayTag RarityTag;
		
	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Info")
	TSubclassOf<UEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Stats")
	FGameplayTag BaseStatTag;

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Stats")
	int32 MinPossibleStats = 0;
	
	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Stats")
	int32 MaxPossibleStats = 1;

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Stats")
	FGameplayTagContainer PossibleStatRolls;

	UPROPERTY(EditDefaultsOnly, Category="Custom Value|Actors")
	TArray<FEquipmentActorsToSpawn> ActorsToSpawn;

	
};
