// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "Equipment/EquipmentDefinition.h"
#include "Equipment/EquipmentInstance.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "Net/UnrealNetwork.h"


UPlayerCharacterGASC* FGFEquipmentList::GetAbilitySystemComponent()
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner());

	return Cast<UPlayerCharacterGASC>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner()));
}

UEquipmentInstance* FGFEquipmentList::AddEntry(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel, int32 Equipped)
{
	check(EquipmentDefinition);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	TSubclassOf<UEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;

	if (!IsValid(InstanceType))
	{
		InstanceType = UEquipmentInstance::StaticClass();
	}

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFEquipmentEntry& Entry = *EntryIt;

		if (Entry.SlotTag.MatchesTagExact(EquipmentCDO->SlotTag))
		{
			RemoveEntry(Entry.Instance);
			break;
		}
	}

	if(Equipped == 1)
	{
		FGFEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

		return NewEntry.Instance;
	}
	
	FGFEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EntryTag = EquipmentCDO->ItemTag;
	NewEntry.SlotTag = EquipmentCDO->SlotTag;
	NewEntry.EnhancementLevel = EnhancementLevel;
	NewEntry.EquipmentDefinition = EquipmentDefinition;	
	NewEntry.StatEffects = StatEffects;
	NewEntry.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	if (NewEntry.HasStats())
	{
		AddEquipmentStats(&NewEntry);
	}

	NewEntry.Instance->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	MarkItemDirty(NewEntry);
	EquipmentEntryDelegate.Broadcast(NewEntry);
	EquippedEntryDelegate.Broadcast(NewEntry);

	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
	                                 FString::Printf(TEXT("Equipped Item: %s"),*NewEntry.EntryTag.ToString()));
	
	return NewEntry.Instance;
}

void FGFEquipmentList::AddEquipmentStats(FGFEquipmentEntry* Entry)
{
	if (UPlayerCharacterGASC* ASC = GetAbilitySystemComponent())
	{
		EquipmentEntryDelegate.Broadcast(*Entry);
		ASC->AddEquipmentEffects(Entry);
	}
}

void FGFEquipmentList::RemoveEquipmentStats(FGFEquipmentEntry* Entry)
{
	if (UPlayerCharacterGASC* ASC = GetAbilitySystemComponent())
	{
		UnEquippedEntryDelegate.Broadcast(*Entry);
		ASC->RemoveEquipmentEffects(Entry);
	}
}


void FGFEquipmentList::RemoveEntry(UEquipmentInstance* EquipmentInstance)
{
	check(OwnerComponent);

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFEquipmentEntry& Entry = *EntryIt;

		if (Entry.Instance == EquipmentInstance)
		{
			Entry.Instance->DestroySpawnedActors();
			RemoveEquipmentStats(&Entry);
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FGFEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		FGFEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);

		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
			FString::Printf(TEXT("UnEquipped Item: %s"),*Entry.EntryTag.ToString()));
	}
}

void FGFEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FGFEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);

		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
			FString::Printf(TEXT("Equipped Item: %s"),*Entry.EntryTag.ToString()));
	}
}

void FGFEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FGFEquipmentEntry& Entry = Entries[Index];

		EquipmentEntryDelegate.Broadcast(Entry);
	}
}

UEquipmentManagerComponent::UEquipmentManagerComponent() :
	EquipmentList(this)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentManagerComponent, EquipmentList);
}

void UEquipmentManagerComponent::EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerEquipItem(EquipmentDefinition, StatEffects, EnhancementLevel);
		return;
	}
	
	int32 Equipped = 0;
	
	if (UEquipmentInstance* Result = EquipmentList.AddEntry(EquipmentDefinition, StatEffects, EnhancementLevel, Equipped))
	{
		Result->OnEquipped();
	}
}

void UEquipmentManagerComponent::InventoryUnEquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
	const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerInventoryUnEquipItem(EquipmentDefinition, StatEffects, EnhancementLevel);
		return;
	}

	int32 Equipped = 1;

	if (UEquipmentInstance* Result = EquipmentList.AddEntry(EquipmentDefinition, StatEffects, EnhancementLevel, Equipped))
	{
		Result->OnEquipped();
	}
}

void UEquipmentManagerComponent::UnEquipItem(UEquipmentInstance* EquipmentInstance)
{
	if (!GetOwner()->HasAuthority())
	{
		ServerUnEquipItem(EquipmentInstance);
		return;
	}

	EquipmentInstance->OnUnEquipped();	
	EquipmentList.RemoveEntry(EquipmentInstance);
}

TArray<FGFEquipmentEntry> UEquipmentManagerComponent::GetEquipmentEntries()
{
	return EquipmentList.Entries;
}

void UEquipmentManagerComponent::ServerEquipItem_Implementation(
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel)
{
	EquipItem(EquipmentDefinition, StatEffects, EnhancementLevel);
}

void UEquipmentManagerComponent::ServerInventoryUnEquipItem_Implementation(
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel)
{
	InventoryUnEquipItem(EquipmentDefinition, StatEffects, EnhancementLevel);
}

void UEquipmentManagerComponent::ServerUnEquipItem_Implementation(UEquipmentInstance* EquipmentInstance)
{
	UnEquipItem(EquipmentInstance);	
}
