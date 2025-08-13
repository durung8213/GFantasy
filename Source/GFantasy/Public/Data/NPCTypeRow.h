// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "GFantasy/GFantasy.h"
#include "NPCTypeRow.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct GFANTASY_API FNPCTypeRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FNPCTypeRow();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	ENPCType D_NPCType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name")
	FText D_NPCTypeName;

};
