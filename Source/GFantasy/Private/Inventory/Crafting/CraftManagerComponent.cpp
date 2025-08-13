// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Crafting/CraftManagerComponent.h"

#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemTypes.h"
#include "Net/UnrealNetwork.h"


void FGFCraftList::GetInventoryComponent()
{
	AActor* OwnerActor = OwnerComponent->GetOwner();
	if (OwnerActor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		InventoryComponent = IInventoryInterface::Execute_GetInventoryComponent(OwnerActor);
	}
}

void FGFCraftList::AddRecipe(const FGameplayTag& RecipeTag, const TArray<FCraftRecipeItemGroup>& RecipeItems)
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFCraftEntry& Entry = *EntryIt;

		if (Entry.EntryTag.MatchesTagExact(RecipeTag))
		{
			return;
		}		
	}

	if (!InventoryComponent)
	{
		GetInventoryComponent();
	}
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	const FMasterItemDefinition Item = InventoryComponent->GetItemDefinitionByTag(RecipeTag);

	FGFCraftEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EntryTag = RecipeTag;
	NewEntry.CategoryTag = Item.CategoryTag;
	NewEntry.CraftedAmount = 1;
	NewEntry.CraftedTag = Item.CraftingProps.CraftedTag;
	NewEntry.RecipeItems = RecipeItems;

	MarkItemDirty(NewEntry);
	CraftRecipeEntryDelegate.Broadcast(NewEntry);

	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Recipe Update");
}

void FGFCraftList::CraftRecipe(const FGFCraftEntry& CraftEntry)
{
	check(OwnerComponent);

	if (!InventoryComponent)
	{
		GetInventoryComponent();
	}
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	if (HasRecipe(CraftEntry.EntryTag) && HasAmountEnough(CraftEntry.RecipeItems))
	{
		InventoryComponent->AddItem(CraftEntry.CraftedTag, CraftEntry.CraftedAmount);
		UseRecipeItem(CraftEntry.RecipeItems);
		CraftingDelegate.Broadcast(CraftEntry);
	}
	
}

void FGFCraftList::UseRecipeItem(const TArray<FCraftRecipeItemGroup>& RecipeItems)
{
	if (!InventoryComponent)
	{
		GetInventoryComponent();
	}
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	TArray<FGFInventoryEntry> InventoryEntries = InventoryComponent->GetInventoryEntries();
	
	for (const FCraftRecipeItemGroup& RecipeItem : RecipeItems)
	{
		for (const FGameplayTag& RecipeTag : RecipeItem.RecipeItemTags)
		{
			for (const FGFInventoryEntry& InventoryEntry : InventoryEntries)
			{
				if (InventoryEntry.ItemTag.MatchesTagExact(RecipeTag))
				{
					InventoryComponent->RemoveItem(RecipeTag, RecipeItem.AmountNeeded);					
				}				
			}			
		}
	}
}

bool FGFCraftList::HasRecipe(const FGameplayTag& RecipeTag)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFCraftEntry& Entry = *EntryIt;

		if (Entry.EntryTag.MatchesTagExact(RecipeTag))
		{
			return true;			
		}
	}

	return false;
}

bool FGFCraftList::HasAmountEnough(const TArray<FCraftRecipeItemGroup>& RecipeItems)
{
	if (!InventoryComponent)
	{
		GetInventoryComponent();
	}
	if (!IsValid(InventoryComponent))
	{
		return false;
	}

	TArray<FGFInventoryEntry> InventoryEntries = InventoryComponent->GetInventoryEntries();
	
	for (const FCraftRecipeItemGroup& RecipeItem : RecipeItems)
	{
		int32 NeededAmount = RecipeItem.AmountNeeded;
		int32 InventoryAmount = 0;

		for (const FGameplayTag& RecipeTag : RecipeItem.RecipeItemTags)
		{
			InventoryAmount += InventoryComponent->GetItemCount(RecipeTag);

			if (InventoryAmount >= NeededAmount)
			{
				break;
			}
		}

		if (InventoryAmount < NeededAmount)
		{
			return false;
		}
	}

	return true;
}

void FGFCraftList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		FGFCraftEntry& Entry = Entries[Index];

		CraftRecipeEntryDelegate.Broadcast(Entry);
	}
}

void FGFCraftList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FGFCraftEntry& Entry = Entries[Index];

		CraftRecipeEntryDelegate.Broadcast(Entry);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("RecipeAdd"));
	}
}

void FGFCraftList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FGFCraftEntry& Entry = Entries[Index];

		CraftRecipeEntryDelegate.Broadcast(Entry);
	}
}

UCraftManagerComponent::UCraftManagerComponent() :
	CraftList(this)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UCraftManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCraftManagerComponent, CraftList);
}

void UCraftManagerComponent::AddRecipe(const FGameplayTag& RecipeTag, const TArray<FCraftRecipeItemGroup>& RecipeItems)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerAddRecipe(RecipeTag, RecipeItems);
		return;
	}

	CraftList.AddRecipe(RecipeTag, RecipeItems);
}

void UCraftManagerComponent::CraftRecipe(const FGFCraftEntry& CraftEntry)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerCraftRecipe(CraftEntry);
		return;
	}

	CraftList.CraftRecipe(CraftEntry);
}

TArray<FGFCraftEntry> UCraftManagerComponent::GetRecipeEntries()
{
	return CraftList.Entries;
}

void UCraftManagerComponent::SaveCraftData(TArray<FCraftSaveData>& OutSaveData) const
{
	OutSaveData.Empty();
	for (const auto& Entry : CraftList.Entries)
	{
		FCraftSaveData Data;
		Data.EntryTag = Entry.EntryTag;
		OutSaveData.Add(Data);
	}
}

void UCraftManagerComponent::ServerAddRecipe_Implementation(const FGameplayTag& RecipeTag, const TArray<FCraftRecipeItemGroup>& RecipeItems)
{
	AddRecipe(RecipeTag, RecipeItems);
}

void UCraftManagerComponent::ServerCraftRecipe_Implementation(const FGFCraftEntry& CraftEntry)
{
	CraftRecipe(CraftEntry);
}
