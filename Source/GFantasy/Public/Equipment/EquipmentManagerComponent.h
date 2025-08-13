// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"


class UPlayerCharacterGASC;
class UEquipmentInstance;
class UEquipmentDefinition;
class UEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FGFEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	FGameplayTag EntryTag = FGameplayTag();

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	FGameplayTag SlotTag = FGameplayTag();

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	int32 EnhancementLevel = 0;

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	TArray<FEquipmentStatEffectGroup> StatEffects = TArray<FEquipmentStatEffectGroup>();

	UPROPERTY(NotReplicated)
	FEquipmentGrantedHandles GrantedHandles = FEquipmentGrantedHandles();

	bool HasStats() const
	{
		return !StatEffects.IsEmpty();
	}

private:

	friend UEquipmentManagerComponent;
	friend struct FGFEquipmentList;

	UPROPERTY()
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition = nullptr;

	UPROPERTY()
	TObjectPtr<UEquipmentInstance> Instance = nullptr;
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEquipmentEntrySignature, const FGFEquipmentEntry& /* 장착 아이템 */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedEntry, const FGFEquipmentEntry& /* 장비창 */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnEquippedEntry, const FGFEquipmentEntry& /* 장착해제 아이템 */);

USTRUCT()
struct FGFEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGFEquipmentList() :
		OwnerComponent(nullptr) {}
	
	FGFEquipmentList(UActorComponent* InComponent) :
		OwnerComponent(InComponent) {}

	UPlayerCharacterGASC* GetAbilitySystemComponent();
	void AddEquipmentStats(FGFEquipmentEntry* Entry);
	void RemoveEquipmentStats(FGFEquipmentEntry* Entry);
	UEquipmentInstance* AddEntry(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel, int32 Equipped);
	void RemoveEntry(UEquipmentInstance* EquipmentInstance);

	// FastArraySerializer 조건
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGFEquipmentEntry, FGFEquipmentList>(Entries, DeltaParms, *this);
	}

	FEquipmentEntrySignature EquipmentEntryDelegate;
	FOnEquippedEntry EquippedEntryDelegate;
	FOnUnEquippedEntry UnEquippedEntryDelegate;

private:

	friend class UEquipmentManagerComponent;

	UPROPERTY()
	TArray<FGFEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
	
};

template<>
struct TStructOpsTypeTraits<FGFEquipmentList> : TStructOpsTypeTraitsBase2<FGFEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};

struct FEquipmentSaveData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GFANTASY_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated)
	FGFEquipmentList EquipmentList;

	UEquipmentManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel);
	void InventoryUnEquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel);
	void UnEquipItem(UEquipmentInstance* EquipmentInstance);

	TArray<FGFEquipmentEntry> GetEquipmentEntries();

private:

	UFUNCTION(Server, Reliable)
	void ServerEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel);

	UFUNCTION(Server, Reliable)
	void ServerInventoryUnEquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel);

	UFUNCTION(Server, Reliable)
	void ServerUnEquipItem(UEquipmentInstance* EquipmentInstance);

};
