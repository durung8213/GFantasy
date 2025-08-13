// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ItemTypesTables.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UItemTypesTables : public UDataAsset
{
	GENERATED_BODY()
	
public:

	/*������ ���̺�*/
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TObjectPtr<UDataTable>> TagToTables;

};
