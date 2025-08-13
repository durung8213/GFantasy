// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Net/UnrealNetwork.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "ItemTypes.h"
#include "ItemTypesTables.h"
#include "Equipment/EquipmentTypes.h"
#include "Data/EquipmentStatEffects.h"
#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "InventoryComponent.generated.h"


class UInventoryComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FEquipmentItemUsed, const TSubclassOf<UEquipmentDefinition>& /* EquipmentDefinition */, const TArray<FEquipmentStatEffectGroup>& /* Stat Effect */, const int32& /* 강화 레벨 */);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FEquipmentItemUnUsed, const TSubclassOf<UEquipmentDefinition>& /* EquipmentDefinition */, const TArray<FEquipmentStatEffectGroup>& /* Stat Effect */, const int32& /* 강화 레벨 */);

// 아이템 태그, 변경된 개수 전달용 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemCountChanged, const FGameplayTag&, ItemTag, const int32, ChangeCount);

USTRUCT(BlueprintType)
struct FGFInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CategoryTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag RarityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	int32 EnhancementLevel = 0;

	UPROPERTY(BlueprintReadOnly)
	FText ItemName = FText();

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity = 0;

	UPROPERTY(BlueprintReadOnly)
	int64 ItemID = 0;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FEquipmentStatEffectGroup> StatEffects = TArray<FEquipmentStatEffectGroup>();	

	UPROPERTY(BlueprintReadOnly)
	int32 Equipped = 0;

	bool IsValid() const
	{
		return ItemID != 0;
	}
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FDirtyInventoryItemSignature, const FGFInventoryEntry& /* DirtyItem */);
DECLARE_MULTICAST_DELEGATE_OneParam(FEquippedItemSignature, const FGFInventoryEntry& /* 장착 아이템 */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCraftRecipeEntry, const FGFInventoryEntry& /* 제작 레시피 */, const TArray<FCraftRecipeItemGroup>& /* 제작 재료 */);
DECLARE_MULTICAST_DELEGATE_OneParam(FInventoryItemRemovedSignature, const int64 /* 아이템 ID */);

USTRUCT()
struct FGFInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGFInventoryList() :
		OwnerComponent(nullptr) {}
	
	FGFInventoryList(UInventoryComponent* InComponent) :
		OwnerComponent(InComponent) {}


	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);
	void RemoveItem(const FGFInventoryEntry& InventoryEntry, int32 NumItems = 1);
	void SellItem(const FGFInventoryEntry& InventoryEntry, int32 NumItems = 1);
	int32 GetItemCount(const FGameplayTag& ItemTag);
	bool HasEnough(const FGameplayTag& ItemTag, int32 NumItems = 1);
	uint64 GenerateID();
	void SetStats(UEquipmentStatEffects* InStats);
	void SetBaseStat(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, FGFInventoryEntry* Entry);
	void RollForStats(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, FGFInventoryEntry* Entry);
	void AddEquippedItem(const FGameplayTag& ItemTag, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel);
	void AddUnEquippedItem(const FGameplayTag& ItemTag, const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel);

	void EnhancementItem(const FGFInventoryEntry& InventoryEntry, int32 NumLevel = 1);
	
	/*FFastArraySerializer의 조건*/
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGFInventoryEntry, FGFInventoryList>(Entries, DeltaParms, *this);
	}

	void LoadItem(const FGameplayTag& ItemTag, int32 NumItems = 1);
	void SaveInventoryListData(TArray<FInventorySaveData>& OutSaveData) const;
	void LoadInventoryListSaveData(const TArray<FInventorySaveData>& SavedItems);


	FDirtyInventoryItemSignature DirtyItemDelegate;
	FEquippedItemSignature EquippedInventoryDelegate;	
	FInventoryItemRemovedSignature InventoryItemRemovedDelegate;	


private:
		
	friend class UInventoryComponent;

	UPROPERTY()
	TArray<FGFInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UInventoryComponent> OwnerComponent;

	UPROPERTY(NotReplicated)
	uint64 LastAssignedID = 0;

	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UEquipmentStatEffects> WeakStats;

};

template<>
struct TStructOpsTypeTraits<FGFInventoryList> : TStructOpsTypeTraitsBase2<FGFInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


//struct FEquipmentSaveData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GFANTASY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	FEquipmentItemUsed EquipmentItemDelegate;
	FEquipmentItemUnUsed UnEquipmentItemDelegate;
	FCraftRecipeEntry AddCraftRecipeDelegate;

	// 아이템 개수 변화에 따른 퀘스트 진행도 변화를 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemCountChanged OnItemCountChanged;

	UPROPERTY(Replicated)
	FGFInventoryList InventoryList;

	UInventoryComponent();
	
	/*리플리케이션할 프로퍼티를 등록하기 위한 오버라이드 함수*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	/*아이템 추가*/
	UFUNCTION(BlueprintCallable)
	void AddItem(const FGameplayTag& ItemTag, int32 NumItems = 1);

	/*아이템 사용*/
	UFUNCTION(BlueprintCallable)
	void UseItem(const FGFInventoryEntry& Entry, int32 NumItems);
	
	/*아이템 제거*/
	UFUNCTION(BlueprintCallable)
	void RemoveItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(BlueprintCallable)
	void SellItem(const FGFInventoryEntry& Entry, int32 NumItems);

	UFUNCTION(BlueprintCallable)
	void EnhancedItem(const FGFInventoryEntry& InventoryEntry, int32 NumItems = 1);

	UFUNCTION(BlueprintCallable)
	void UnEquippedItem(const FGFInventoryEntry& Entry, int32 NumItems);

	// 인벤토리내 특정 아이템 개수 검색
	UFUNCTION(BlueprintCallable)
	int32 GetItemCount(const FGameplayTag& ItemTag);

	/*Tag로 정의된 아이템 정의를 찾는 함수*/
	UFUNCTION(BlueprintPure)
	FMasterItemDefinition GetItemDefinitionByTag(const FGameplayTag& ItemTag) const;

	UFUNCTION(BlueprintPure)
	FEquipmentStatEffectGroup GetEquipmentStatEffectGroupByTag(const FGameplayTag& ItemTag) const;

	UFUNCTION(BlueprintCallable)
	TArray<FGFInventoryEntry> GetInventoryEntries();
	
	void AddEquippedItemEntry(const FGameplayTag& ItemTag, const TArray<FEquipmentStatEffectGroup>& InStatEffects, const int32& EnhancementLevel);
	void AddUnEquippedItemEntry(const FGameplayTag& ItemTag, const TArray<FEquipmentStatEffectGroup>& InStatEffects, const int32& EnhancementLevel);

public:
	/////////////////////////////// SaveGame ////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveInventoryData(TArray<FInventorySaveData>& OutSaveData) const;

	// 페이징 로드를 시작할 때 총 인벤토리 아이템의 개수를 알려줌
	void PrepareForLoad(int32 TotalItems);

	// 페이지 단위로 넘어온 데이터를 받아 누적 로드
	void LoadInventoryPage(int32 PageIndex, const TArray<FInventorySaveData>& InvPage);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadInventorySaveData(const TArray<FInventorySaveData>& SavedItems);


	// 페이징 로드를 시작할 때 총 제작법의 아이템의 개수를 알려줌
	void PrepareCraftForLoad(int32 TotalCrafts);

	// 페이지 단위로 넘어온 데이터를 받아 누적 로드
	void LoadCraftPage(int32 PageIndex, const TArray<FCraftSaveData>& CraftPage);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadCraftSaveData(const TArray<FCraftSaveData>& SavedCrafts);


private:
	// --- 서버 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_PrepareForLoad(int32 TotalItems);

	UFUNCTION(Server, Reliable)
	void Server_LoadInventoryPage(int32 PageIndex, const TArray<FInventorySaveData>& InvPage);
	
	UFUNCTION(Server, Reliable)
	void Server_LoadInventorySaveData(const TArray<FInventorySaveData>& SavedItems);

	// --- 서버 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_PrepareCraftForLoad(int32 TotalCrafts);

	UFUNCTION(Server, Reliable)
	void Server_LoadCraftPage(int32 PageIndex, const TArray<FCraftSaveData>& CraftPage);
	
	UFUNCTION(Server, Reliable)
	void Server_LoadCraftSaveData(const TArray<FCraftSaveData>& SavedCrafts);


private:
	// 페이징 중간 데이터를 저장할 임시 배열
	UPROPERTY()
	TArray<FInventorySaveData> PendingLoadData;

	// 총 로드해야 할 아이템 수
	UPROPERTY()
	int32 ExpectedItems = 0;


	// 페이징 중간 데이터를 저장할 임시 배열
	UPROPERTY()
	TArray<FCraftSaveData> PendingCraftLoadData;

	// 총 로드해야 할 제작법 수
	UPROPERTY()
	int32 ExpectedCrafts = 0;


private:

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Stat Effects")
	TObjectPtr<UEquipmentStatEffects> StatEffects;

	/*아이템 정의 테이블*/
	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Item Definition")
	TObjectPtr<UItemTypesTables> InventoryDefinition;

	/*아이템 추가 서버 RPC*/
	UFUNCTION(Server, Reliable)
	void ServerAddItem(const FGameplayTag& ItemTag, int32 NumItems);

	/*아이템 사용 서버 RPC*/
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseItem(const FGFInventoryEntry& Entry, int32 NumItems);

	bool ServerUseItem_Validate(const FGFInventoryEntry& Entry, int32 NumItems);

	/*아이템 제거 서버 RPC*/
	UFUNCTION(Server, Reliable)
	void ServerRemoveItem(const FGameplayTag& ItemTag, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerSellItem(const FGFInventoryEntry& Entry, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerEnhancedItem(const FGFInventoryEntry& InventoryEntry, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerUnEquippedItem(const FGFInventoryEntry& Entry, int32 NumItems);

	UFUNCTION(Server, Reliable)
	void ServerGetItemCount(const FGameplayTag& ItemTag);

};