// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetControllers/CraftWidgetController.h"

#include "Inventory/Crafting/CraftManagerComponent.h"
#include "Interface/InventoryInterface.h"


void UCraftWidgetController::SetOwningActor(AActor* InOwner)
{
	OwningActor = InOwner;
}

void UCraftWidgetController::BindCallbacksToDependencies()
{
	OwningCrafting = IInventoryInterface::Execute_GetCraftComponent(OwningActor);

	if (IsValid(OwningCrafting))
	{
		OwningCrafting->CraftList.CraftRecipeEntryDelegate.AddLambda(
			[this] (const FGFCraftEntry& Entry)
			{
				CraftEntryDelegate.Broadcast(Entry);
			});
	}	
}

void UCraftWidgetController::BroadcastInitialValues()
{
	if (IsValid(OwningCrafting))
	{
		for (const FGFCraftEntry& Entry : OwningCrafting->GetRecipeEntries())
		{
			CraftEntryDelegate.Broadcast(Entry);
		}
	}
}
