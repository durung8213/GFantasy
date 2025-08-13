// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "CustomizeDefinition.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FCustomizeDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CustomizeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SkeletalMeshName = FName();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName CustomizeName = FName();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> CustomMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> SubCustomMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> CustomMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor CustomMaterialColor = FLinearColor::Black;
};