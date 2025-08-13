// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetControllers/WidgetController.h"
#include "Inventory/Crafting/CraftManagerComponent.h"
#include "CraftWidgetController.generated.h"


struct FGFInventoryEntry;
class UInventoryComponent;
struct FGFCraftEntry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCraftRecipeSignature, const FGFCraftEntry&, Entry);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GFANTASY_API UCraftWidgetController : public UWidgetController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FCraftRecipeSignature CraftEntryDelegate;

	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	void BroadcastInitialValues();

private:

	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;

	UPROPERTY()
	TObjectPtr<UCraftManagerComponent> OwningCrafting;
	
};
