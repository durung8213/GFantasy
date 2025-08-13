// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "InventoryInterface.generated.h"

class UCraftManagerComponent;
class UEquipmentManagerComponent;
class UInventoryComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GFANTASY_API IInventoryInterface
{
	GENERATED_BODY()

	
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UInventoryComponent* GetInventoryComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UEquipmentManagerComponent* GetEquipmentComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCraftManagerComponent* GetCraftComponent();
	
};
