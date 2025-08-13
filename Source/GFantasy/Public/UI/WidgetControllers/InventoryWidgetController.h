// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetControllers/WidgetController.h"
#include "InventoryWidgetController.generated.h"


struct FGFEquipmentEntry;
class UEquipmentManagerComponent;
struct FGFInventoryEntry;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryEntrySignature, const FGFInventoryEntry&, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedInventorySignature, const FGFInventoryEntry&, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemRemoved, const int64, ItemID);
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GFANTASY_API UInventoryWidgetController : public UWidgetController
{
	GENERATED_BODY()
	

public:

	UPROPERTY(BlueprintAssignable)
	FInventoryEntrySignature InventoryEntryDelegate;

	UPROPERTY(BlueprintAssignable)
	FEquippedInventorySignature EquippedInventoryDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemRemoved OnInventoryItemRemoved;

	void SetOwningActor(AActor* InOwner);

	void BindCallbacksToDependencies();
	void BroadcastInitialValues();

	
private:	
	
	UPROPERTY()
	TObjectPtr<AActor> OwningActor;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> OwningInventory;

	UPROPERTY()
	TObjectPtr<UEquipmentManagerComponent> OwningEquipment;
	
};
