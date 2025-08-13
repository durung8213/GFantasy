// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/InventoryWidgetController.h"

#include "Inventory/InventoryComponent.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "Interface/InventoryInterface.h"

void UInventoryWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UInventoryWidgetController::BindCallbacksToDependencies()
{
	OwningInventory = IInventoryInterface::Execute_GetInventoryComponent(OwningActor);	

	if (IsValid(OwningInventory))
	{
		OwningInventory->InventoryList.DirtyItemDelegate.AddLambda(
			[this](const FGFInventoryEntry& DirtyItem)
			{
				InventoryEntryDelegate.Broadcast(DirtyItem);
			});
		OwningInventory->InventoryList.EquippedInventoryDelegate.AddLambda(
			[this](const FGFInventoryEntry& DirtyItem)
			{
				EquippedInventoryDelegate.Broadcast(DirtyItem);
			});
		OwningInventory->InventoryList.InventoryItemRemovedDelegate.AddLambda(
			[this] (const int64 ItemID)
			{
				OnInventoryItemRemoved.Broadcast(ItemID);
			});
	}
}

void UInventoryWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningInventory))
	{
		for (const FGFInventoryEntry& Entry : OwningInventory->GetInventoryEntries())
		{
			InventoryEntryDelegate.Broadcast(Entry);
		}
		/*for (const FGFInventoryEntry& Entry : OwningInventory->GetInventoryEntries())
		{
			EquippedInventoryDelegate.Broadcast(Entry);
		}*/
	}
}