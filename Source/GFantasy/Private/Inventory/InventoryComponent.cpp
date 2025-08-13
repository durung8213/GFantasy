// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "GFantasy/Public/Libraries/GASAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NativeGameplayTags.h"
#include "Chaos/PBDRigidsSOAs.h"
#include "Controller/GASWorldPlayerController.h"
#include "Equipment/EquipmentDefinition.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "Inventory/Crafting/CraftManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/GASBasePlayerState.h"
#include "Quest/QuestManagerComponent.h"
#include "SaveGame/PlayerSaveData.h"

namespace GFGameplayTags::Static
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Equipment, "Item.Equipment");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Craft, "Item.Craft");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(Category_Cash, "Item.Cash");
}

void FGFInventoryList::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	if (ItemTag.MatchesTag(GFGameplayTags::Static::Category_Equipment))
	{		
	}
	else
	{
		for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
		{
			FGFInventoryEntry& Entry = *EntryIt;

			if (Entry.ItemTag.MatchesTagExact(ItemTag))
			{
				Entry.Quantity += NumItems;
				
				MarkItemDirty(Entry);

				// PC 서버 권한일 때 실행
				if (OwnerComponent->GetOwner()->HasAuthority())
				{
					DirtyItemDelegate.Broadcast(Entry);

					// 컴포넌트 레벨 델리게이트로도 포워드
					OwnerComponent->OnItemCountChanged.Broadcast(Entry.ItemTag, Entry.Quantity);
				}

				return;
			}
		}	
	}	
	
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FGFInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.CategoryTag = Item.CategoryTag;
	NewEntry.RarityTag = Item.RarityTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = NumItems;
	NewEntry.ItemID = GenerateID();

	// GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Black,FString::Printf(TEXT("Item: %s"),*NewEntry.ItemTag.ToString()));

	if (NewEntry.ItemTag.MatchesTag(GFGameplayTags::Static::Category_Equipment) && IsValid(WeakStats.Get()))
	{		
		SetBaseStat(Item.EquipmentItemProps.EquipmentClass, &NewEntry);
		RollForStats(Item.EquipmentItemProps.EquipmentClass, &NewEntry);
	}

	if (OwnerComponent->GetOwner()->HasAuthority())
	{
		DirtyItemDelegate.Broadcast(NewEntry);

		// 컴포넌트 레벨 델리게이트로도 포워드
		OwnerComponent->OnItemCountChanged.Broadcast(NewEntry.ItemTag, NewEntry.Quantity);
	}

	MarkItemDirty(NewEntry);
}

void FGFInventoryList::SetBaseStat(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition,
	FGFInventoryEntry* Entry)
{
	UEquipmentStatEffects* StatEffects = WeakStats.Get();
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);
	const FGameplayTag& BaseStatTag = EquipmentCDO->BaseStatTag;
	
	Entry->StatEffects.RemoveAll([&](const FEquipmentStatEffectGroup& Effect)
	{
		return Effect.StatEffectTag.MatchesTag(BaseStatTag);
	});

	if (BaseStatTag.IsValid())
	{
		for (const auto& Pair : StatEffects->MasterStatMap)
		{
			if (BaseStatTag.MatchesTag(Pair.Key))
			{
				if (const FEquipmentStatEffectGroup* BaseStat = UGASAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value, BaseStatTag))
				{
					FEquipmentStatEffectGroup NewStat = *BaseStat;

					NewStat.CurrentValue = BaseStat->bFractionalStat ? (BaseStat->BaseStatLevel + (BaseStat->EnhancementValue * Entry->EnhancementLevel)) :
					FMath::TruncToInt(BaseStat->BaseStatLevel + (BaseStat->EnhancementValue * Entry->EnhancementLevel));

					Entry->StatEffects.Add(NewStat);
					break;
				}
			}
		}
	}
}

void FGFInventoryList::RollForStats(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, FGFInventoryEntry* Entry)
{
	UEquipmentStatEffects* StatEffects = WeakStats.Get();
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

	const int32 NumStatsToRoll = FMath::RandRange(EquipmentCDO->MinPossibleStats, EquipmentCDO->MaxPossibleStats);
	int32 StatRollIndex = 0;
	while (StatRollIndex < NumStatsToRoll)
	{
		const int32 RandomIndex = FMath::RandRange(0, EquipmentCDO->PossibleStatRolls.Num() - 1);
		const FGameplayTag& RandomTag = EquipmentCDO->PossibleStatRolls.GetByIndex(RandomIndex);

		for (const auto& Pair :	StatEffects->MasterStatMap)
		{
			if (RandomTag.MatchesTag(Pair.Key))
			{
				if (const FEquipmentStatEffectGroup* PossibleStat = UGASAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value, RandomTag))
				{
					if (FMath::FRandRange(0.f,1.f) < PossibleStat->ProbabilityToSelect)
					{
						FEquipmentStatEffectGroup NewStat = *PossibleStat;

						NewStat.CurrentValue = PossibleStat->bFractionalStat ? FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel) :
						FMath::TruncToInt(FMath::FRandRange(PossibleStat->MinStatLevel, PossibleStat->MaxStatLevel));

						Entry->StatEffects.Add(NewStat);
						++StatRollIndex;
					}
				}
			}
		}
	}
}

void FGFInventoryList::AddEquippedItem(const FGameplayTag& ItemTag,
	const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel)
{
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FGFInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.CategoryTag = Item.CategoryTag;
	NewEntry.RarityTag = Item.RarityTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = 1;
	NewEntry.EnhancementLevel = EnhancementLevel;
	NewEntry.ItemID = GenerateID();
	NewEntry.Equipped = 1;
	NewEntry.StatEffects = StatEffects;

	DirtyItemDelegate.Broadcast(NewEntry);
	MarkItemDirty(NewEntry);
}

void FGFInventoryList::AddUnEquippedItem(const FGameplayTag& ItemTag,
	const TArray<FEquipmentStatEffectGroup>& StatEffects, const int32& EnhancementLevel)
{
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(ItemTag);
	
	FGFInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemTag = ItemTag;
	NewEntry.CategoryTag = Item.CategoryTag;
	NewEntry.RarityTag = Item.RarityTag;
	NewEntry.ItemName = Item.ItemName;
	NewEntry.Quantity = 1;
	NewEntry.EnhancementLevel = EnhancementLevel;
	NewEntry.ItemID = GenerateID();
	NewEntry.Equipped = 0;
	NewEntry.StatEffects = StatEffects;

	DirtyItemDelegate.Broadcast(NewEntry);
	MarkItemDirty(NewEntry);
}

void FGFInventoryList::EnhancementItem(const FGFInventoryEntry& InventoryEntry, int32 NumLevel)
{
	if (InventoryEntry.ItemTag.MatchesTag(GFGameplayTags::Static::Category_Equipment))
	{
	}
	else
	{
		return;		
	}		

	if (InventoryEntry.Equipped != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Equipped Item"));
		return;
	}
	
	const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(InventoryEntry.ItemTag);
	
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemID == InventoryEntry.ItemID)
		{
			Entry.EnhancementLevel += NumLevel;
			SetBaseStat(Item.EquipmentItemProps.EquipmentClass, &Entry);
			MarkItemDirty(Entry);
			DirtyItemDelegate.Broadcast(Entry);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Enhancement Item"));
		}
	}	
}

void FGFInventoryList::RemoveItem(const FGFInventoryEntry& InventoryEntry, int32 NumItems)
{
	// 인벤토리 내에서 아이템 사용 등으로 줄어들때 쓰는 함수
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFInventoryEntry& Entry = *EntryIt;
		
		if (Entry.ItemID == InventoryEntry.ItemID)
		{
			Entry.Quantity -= NumItems;

			OwnerComponent->OnItemCountChanged.Broadcast(Entry.ItemTag, Entry.Quantity);

			if (Entry.Quantity > 0)
			{
				// 있을 때
				MarkItemDirty(Entry);

				if (OwnerComponent->GetOwner()->HasAuthority())
				{
					DirtyItemDelegate.Broadcast(Entry);
				}
			}
			else
			{
				if (InventoryEntry.ItemTag.MatchesTagExact(GFGameplayTags::Static::Category_Cash))
				{
					MarkItemDirty(Entry);

					if (OwnerComponent->GetOwner()->HasAuthority())
					{
						DirtyItemDelegate.Broadcast(Entry);						
					}
				}
				else
				{
					// 개수가 0이 될때
					InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
					EntryIt.RemoveCurrent();
					MarkArrayDirty();
				}				
			}						
			break;
		}
	}
}

void FGFInventoryList::SellItem(const FGFInventoryEntry& InventoryEntry, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemID == InventoryEntry.ItemID)
		{
			Entry.Quantity -= NumItems;
			
			if (Entry.Quantity > 0)
			{
				// 있을 때
				MarkItemDirty(Entry);

				if (OwnerComponent->GetOwner()->HasAuthority())
				{
					DirtyItemDelegate.Broadcast(Entry);

					// 퀘스트에게 알려주는 역할
					OwnerComponent->OnItemCountChanged.Broadcast(Entry.ItemTag, Entry.Quantity);
				}
			}
			else
			{
				// 개수가 0이 될때
				InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
				EntryIt.RemoveCurrent();
				MarkArrayDirty();
			}
			break;
		}		
	}
}

int32 FGFInventoryList::GetItemCount(const FGameplayTag& ItemTag)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTagExact(ItemTag))
		{			
			return Entry.Quantity;
		}
	}

	return 0;
}

bool FGFInventoryList::HasEnough(const FGameplayTag& ItemTag, int32 NumItems)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFInventoryEntry& Entry = *EntryIt;
		
		if (Entry.ItemTag.MatchesTagExact(ItemTag))
		{
			if (Entry.Quantity >= NumItems)
			{
				return true;
			}
		}
	}

	return false;
}

uint64 FGFInventoryList::GenerateID()
{
	uint64 NewID = ++LastAssignedID;

	int32 SignatureIndex = 0;
	while (SignatureIndex < 12)
	{
		if (FMath::RandRange(0, 100) < 85)
		{
			NewID |= (uint64)1 << FMath::RandRange(0, 63);
		}
		++SignatureIndex;
	}

	return NewID;
}

void FGFInventoryList::SetStats(UEquipmentStatEffects* InStats)
{
	WeakStats = InStats;
}



void FGFInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// 아이템이 인벤토리에서 아예없어질때 실행되는 함수
	for (const int32 Index : RemovedIndices)
	{
		const FGFInventoryEntry& Entry = Entries[Index];

		InventoryItemRemovedDelegate.Broadcast(Entry.ItemID);
	}
}

void FGFInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	// 아이템이 인벤토리에 추가될때 실행되는 함수
	for (const int32 Index : AddedIndices)
	{
		FGFInventoryEntry& Entry = Entries[Index];
		
		DirtyItemDelegate.Broadcast(Entry);
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("Add"));
	}
}

void FGFInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	// 인벤토리내 아이템의 변동이 있을때
	for (const int32 Index : ChangedIndices)
	{
		FGFInventoryEntry& Entry = Entries[Index];

		DirtyItemDelegate.Broadcast(Entry);
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("Quantity : %d"), Entry.Quantity));
	}
}

void FGFInventoryList::SaveInventoryListData(TArray<FInventorySaveData>& OutSaveData) const
{
	OutSaveData.Empty();

	// Entries 배열을 순회하며 FInventorySaveData 구조체로 변환
	for (const FGFInventoryEntry& Item : Entries)
	{
		FInventorySaveData IData;
		IData.ItemTag = Item.ItemTag;
		IData.CategoryTag = Item.CategoryTag;
		IData.EnhancementLevel = Item.EnhancementLevel;
		IData.ItemName = Item.ItemName;
		IData.Quantity = Item.Quantity;
		IData.ItemID = Item.ItemID;
		IData.StatEffects = Item.StatEffects;
		IData.Equipped = Item.Equipped;

		OutSaveData.Add(IData);
	}
}

///////////////////////////////////////////////////////////////////////////
// Save & Load

void FGFInventoryList::LoadInventoryListSaveData(const TArray<FInventorySaveData>& SavedItems)
{
	if (!OwnerComponent) return;

	// 1) 기존 Entries 전부 RemoveItem 호출로 제거
	//    ranged-for 대신 인덱스 역순 순회로 변경
	for (int32 Index = Entries.Num() - 1; Index >= 0; --Index)
	{
		const FGFInventoryEntry& Entry = Entries[Index];
		OwnerComponent->RemoveItem(Entry.ItemTag, Entry.Quantity);
	}

	// 2) 저장된 데이터 순회하며 AddItem/Equip 호출
	for (const FInventorySaveData& SaveData : SavedItems)
	{
		const FMasterItemDefinition Item = OwnerComponent->GetItemDefinitionByTag(SaveData.ItemTag);
		if (IsValid(Item.EquipmentItemProps.EquipmentClass))
		{
			// 장비 장착을 위한 코드
			// 장비 아이템 등록
			// Equipped == 0일 때 -> Add
			if (SaveData.Equipped == 0)
			{
				AddUnEquippedItem(SaveData.ItemTag, SaveData.StatEffects, SaveData.EnhancementLevel);
			}
			else
			{
				OwnerComponent->EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass, SaveData.StatEffects, SaveData.EnhancementLevel);
			}
			// Equipped != 0일 때 -> Equipped를 0으로 바꾸고 Use
		}
		else
		{
			OwnerComponent->AddItem(SaveData.ItemTag, SaveData.Quantity);
		}
	}
}

UInventoryComponent::UInventoryComponent() :
	InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		InventoryList.SetStats(StatEffects);		
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
}

void UInventoryComponent::AddItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	// 이게 서버체크 아이템 추가 함수
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return; /*오너 없음*/

	if (!Owner->HasAuthority()) /*클라*/
	{
		/*서버 RPC 요청*/
		ServerAddItem(ItemTag, NumItems);
		return;
	}

	InventoryList.AddItem(ItemTag, NumItems);
}

void UInventoryComponent::ServerAddItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	AddItem(ItemTag, NumItems);
}

void UInventoryComponent::UseItem(const FGFInventoryEntry& Entry, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return; /*오너 없음*/

	if (!Owner->HasAuthority()) /*클라*/
	{
		/*서버 RPC 요청*/
		ServerUseItem(Entry, NumItems);
		return;
	}	

	if (InventoryList.HasEnough(Entry.ItemTag, NumItems))
	{
		/*아이템 정의 확인*/
		const FMasterItemDefinition Item = GetItemDefinitionByTag(Entry.ItemTag);

		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner)) /*인벤토리 컴포넌트의 오너의 ASC*/
		{
			if (IsValid(Item.ConsumableProps.ItemEffectClass)) /*소비템 EffectClass 유효*/
			{
				/*컨텍스트 생성->스펙 생성->오너에게 적용*/
				const FGameplayEffectContextHandle ContextHandle = OwnerASC->MakeEffectContext();
				
				const FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(Item.ConsumableProps.ItemEffectClass,
					Item.ConsumableProps.ItemEffectLevel, ContextHandle);
				
				OwnerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				/*1개 소비*/
				InventoryList.RemoveItem(Entry, NumItems);				

				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT("Server Item Used: %s"), *Item.ItemTag.ToString()));
			}
			
			if (Item.CraftingProps.RecipeItems.Num() > 0)
			{
				AddCraftRecipeDelegate.Broadcast(Entry, Item.CraftingProps.RecipeItems);
				InventoryList.RemoveItem(Entry, NumItems);
			}

			if (Entry.Equipped == 0)
			{
				if (IsValid(Item.EquipmentItemProps.EquipmentClass))
				{				
					EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass, Entry.StatEffects, Entry.EnhancementLevel);				
					InventoryList.RemoveItem(Entry);
				}
			}
			else
			{
				if (IsValid(Item.EquipmentItemProps.EquipmentClass))
				{				
					UnEquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass, Entry.StatEffects, Entry.EnhancementLevel);
					InventoryList.RemoveItem(Entry);
				}
			}			
		}
	}
}

void UInventoryComponent::RemoveItem(const FGameplayTag& ItemTag, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return; /*오너 없음*/

	if (!Owner->HasAuthority()) /*클라*/
	{
		/*서버 RPC 요청*/
		ServerRemoveItem(ItemTag, NumItems);
		return;
	}

	for (auto EntryIt = InventoryList.Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFInventoryEntry& Entry = *EntryIt;

		if (Entry.ItemTag.MatchesTagExact(ItemTag))
		{
			InventoryList.RemoveItem(Entry, NumItems);
		}
	}	
}

void UInventoryComponent::SellItem(const FGFInventoryEntry& Entry, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerSellItem(Entry, NumItems);
		return;
	}
	
	InventoryList.SellItem(Entry, NumItems);
}

void UInventoryComponent::EnhancedItem(const FGFInventoryEntry& InventoryEntry, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerEnhancedItem(InventoryEntry, NumItems);
		return;
	}

	InventoryList.EnhancementItem(InventoryEntry, NumItems);
}

void UInventoryComponent::UnEquippedItem(const FGFInventoryEntry& Entry, int32 NumItems)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return; /*오너 없음*/

	if (!Owner->HasAuthority()) /*클라*/
	{
		/*서버 RPC 요청*/
		ServerUnEquippedItem(Entry, NumItems);
		return;
	}

	if (InventoryList.HasEnough(Entry.ItemTag, NumItems))
	{
		/*아이템 정의 확인*/
		const FMasterItemDefinition Item = GetItemDefinitionByTag(Entry.ItemTag);

		if (Entry.Equipped == 0)
		{			
			EquipmentItemDelegate.Broadcast(Item.EquipmentItemProps.EquipmentClass, Entry.StatEffects, Entry.EnhancementLevel);
		}

		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner)) /*인벤토리 컴포넌트의 오너의 ASC*/
		{						
						
			InventoryList.RemoveItem(Entry);
		}
	}	
}

int32 UInventoryComponent::GetItemCount(const FGameplayTag& ItemTag)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return 0; /*오너 없음*/

	if (!Owner->HasAuthority()) /*클라*/
	{
		/*서버 RPC 요청*/
		ServerGetItemCount(ItemTag);
		return 0;
	}

	return InventoryList.GetItemCount(ItemTag);
}

void UInventoryComponent::ServerUseItem_Implementation(const FGFInventoryEntry& Entry, int32 NumItems)
{
	UseItem(Entry, NumItems);
}

FMasterItemDefinition UInventoryComponent::GetItemDefinitionByTag(const FGameplayTag& ItemTag) const
{
	checkf(InventoryDefinition, TEXT("No Inven Def Inside Comp %s"), *GetNameSafe(this)); /*정의 유효*/

	for (const auto& Pair : InventoryDefinition->TagToTables)
	{
		/*테이블에서 Tag로 조회*/
		if (ItemTag.MatchesTag(Pair.Key))
		{
			if (const FMasterItemDefinition* ValidItem = UGASAbilitySystemLibrary::GetDataTableRowByTag<FMasterItemDefinition>(Pair.Value, ItemTag))
			{
				return *ValidItem;
			}
		}
	}

	return FMasterItemDefinition();
}

FEquipmentStatEffectGroup UInventoryComponent::GetEquipmentStatEffectGroupByTag(const FGameplayTag& ItemTag) const
{
	checkf(InventoryDefinition, TEXT("No Equip Def Inside Comp %s"), *GetNameSafe(this)); /*정의 유효*/

	for (const auto& Pair : StatEffects->MasterStatMap)
	{
		if (ItemTag.MatchesTag(Pair.Key))
		{
			if (const FEquipmentStatEffectGroup* ValidItem = UGASAbilitySystemLibrary::GetDataTableRowByTag<FEquipmentStatEffectGroup>(Pair.Value, ItemTag))
			{
				return *ValidItem;
			}
		}
	}

	return FEquipmentStatEffectGroup();
}

TArray<FGFInventoryEntry> UInventoryComponent::GetInventoryEntries()
{
	return InventoryList.Entries;
}

void UInventoryComponent::AddEquippedItemEntry(const FGameplayTag& ItemTag,
	const TArray<FEquipmentStatEffectGroup>& InStatEffects, const int32& EnhancementLevel)
{
	InventoryList.AddEquippedItem(ItemTag, InStatEffects, EnhancementLevel);
}

void UInventoryComponent::AddUnEquippedItemEntry(const FGameplayTag& ItemTag,
	const TArray<FEquipmentStatEffectGroup>& InStatEffects, const int32& EnhancementLevel)
{
	InventoryList.AddUnEquippedItem(ItemTag, InStatEffects, EnhancementLevel);
}

// 인벤토리 저장 함수
void UInventoryComponent::SaveInventoryData(TArray<FInventorySaveData>& OutSaveData) const
{
	OutSaveData.Empty();
	for (const auto& Entry : InventoryList.Entries)
	{
		FInventorySaveData Data;
		Data.ItemTag = Entry.ItemTag;
		Data.CategoryTag = Entry.ItemTag;
		Data.EnhancementLevel = Entry.EnhancementLevel;
		Data.ItemName = Entry.ItemName;
		Data.Quantity = Entry.Quantity;
		Data.ItemID = Entry.ItemID;
		Data.StatEffects = Entry.StatEffects;
		Data.Equipped = Entry.Equipped;
		OutSaveData.Add(Data);
	}
}


void UInventoryComponent::PrepareForLoad(int32 TotalItems)
{
	// 클라이언트일 경우: 서버로 RPC 요청
	if (!GetOwner()->HasAuthority())
	{
		Server_PrepareForLoad(TotalItems);
		return;
	}

	// 페이징 임시 저장소 초기화
	PendingLoadData.Empty();

	ExpectedItems = TotalItems;
}

void UInventoryComponent::LoadInventoryPage(int32 PageIndex, const TArray<FInventorySaveData>& InvPage)
{
	// 클라이언트일 경우: 서버로 RPC 요청
	if (!GetOwner()->HasAuthority())
	{
		Server_LoadInventoryPage(PageIndex, InvPage);
		return;
	}

	// 들어온 페이지 데이터를 임시 배열에 누적
	PendingLoadData.Append(InvPage);

	// 마지막 페이지까지 모두 수신했으면 실제 로드 수행
	if (PendingLoadData.Num() >= ExpectedItems)
	{
		LoadInventorySaveData(PendingLoadData);
		// 클리어해 두면, 같은 세션에 다시 로드해도 안전합니다.
		PendingLoadData.Empty();
	}
}

void UInventoryComponent::LoadInventorySaveData(const TArray<FInventorySaveData>& SavedItems)
{
	// 클라이언트일 경우: 서버로 RPC 요청
	if (!GetOwner()->HasAuthority())
	{
		Server_LoadInventorySaveData(SavedItems);
		return;
	}

	// **여기서는 오직 이 한 줄만!**
	InventoryList.LoadInventoryListSaveData(SavedItems);

	// 서버에서 바로 최종 적용
	PendingLoadData = SavedItems;
	ExpectedItems = 0;
}

/////////////////////////////////////////////////////////////////////
void UInventoryComponent::PrepareCraftForLoad(int32 TotalCrafts)
{	
	// 클라이언트일 경우: 서버로 RPC 요청
	if (!GetOwner()->HasAuthority())
	{
		Server_PrepareCraftForLoad(TotalCrafts);
		return;
	}

	// 페이징 임시 저장소 초기화
	PendingCraftLoadData.Empty();

	ExpectedCrafts = TotalCrafts;
}

void UInventoryComponent::LoadCraftPage(int32 PageIndex, const TArray<FCraftSaveData>& CraftPage)
{	
	// 클라이언트일 경우: 서버로 RPC 요청
	if (!GetOwner()->HasAuthority())
	{
		Server_LoadCraftPage(PageIndex, CraftPage);
		return;
	}

	// 들어온 페이지 데이터를 임시 배열에 누적
	PendingCraftLoadData.Append(CraftPage);

	// 마지막 페이지까지 모두 수신했으면 실제 로드 수행
	if (PendingCraftLoadData.Num() >= ExpectedCrafts)
	{
		LoadCraftSaveData(PendingCraftLoadData);
		// 클리어해 두면, 같은 세션에 다시 로드해도 안전합니다.
		PendingCraftLoadData.Empty();
	}
}

void UInventoryComponent::LoadCraftSaveData(const TArray<FCraftSaveData>& SavedCrafts)
{	
	// 클라이언트일 경우: 서버로 RPC 요청
	if (!GetOwner()->HasAuthority())
	{
		Server_LoadCraftSaveData(SavedCrafts);
		return;
	}

	// 저장해놓은 제작법 태그로 엔트리를 만들어주고 인벤토리 컴포넌의 AddItem, UseItem사용해서 제작법 등록
	// 저장된 Craft 태그만큼 반복
	for (const FCraftSaveData& CraftData : SavedCrafts)
	{
		// 인벤토리에 Craft 아이템 1개 추가
		AddItem(CraftData.EntryTag, 1);
		UE_LOG(LogTemp, Log, TEXT("LoadCraftSaveData: Added craft item %s"), *CraftData.EntryTag.ToString());

		// 방금 추가된 엔트리를 찾아 UseItem 호출
		// UseItem 내부에서 CraftProps.RecipeItems가 있으면 AddCraftRecipeDelegate로 제작법이 등록됩니다.
		FGFInventoryEntry TempEntry;
		TempEntry.ItemTag = CraftData.EntryTag;
		TempEntry.Quantity = 1;
		TempEntry.ItemID = 0;             // ID는 중요하지 않으므로 0으로 둡니다
		TempEntry.StatEffects.Empty();    // 제작법은 StatEffects 없으므로 빈 배열
		UseItem(TempEntry, 1);
		UE_LOG(LogTemp, Log, TEXT("LoadCraftSaveData: Used craft item %s to register recipe"), *CraftData.EntryTag.ToString());
	}

	// 서버에서 바로 최종 적용
	PendingCraftLoadData = SavedCrafts;
	ExpectedCrafts = 0;
}


void UInventoryComponent::Server_PrepareForLoad_Implementation(int32 TotalItems)
{
	PrepareForLoad(TotalItems);
}

void UInventoryComponent::Server_LoadInventoryPage_Implementation(int32 PageIndex, const TArray<FInventorySaveData>& InvPage)
{
	LoadInventoryPage(PageIndex, InvPage);
}

void UInventoryComponent::Server_LoadInventorySaveData_Implementation(const TArray<FInventorySaveData>& SavedItems)
{
	LoadInventorySaveData(SavedItems);
}

void UInventoryComponent::Server_PrepareCraftForLoad_Implementation(int32 TotalCrafts)
{
	PrepareCraftForLoad(TotalCrafts);
}

void UInventoryComponent::Server_LoadCraftPage_Implementation(int32 PageIndex, const TArray<FCraftSaveData>& CraftPage)
{
	LoadCraftPage(PageIndex, CraftPage);
}

void UInventoryComponent::Server_LoadCraftSaveData_Implementation(const TArray<FCraftSaveData>& SavedCrafts)
{
	LoadCraftSaveData(SavedCrafts);
}


bool UInventoryComponent::ServerUseItem_Validate(const FGFInventoryEntry& Entry, int32 NumItems)
{
	return Entry.IsValid() && InventoryList.HasEnough(Entry.ItemTag, NumItems);
}

void UInventoryComponent::ServerSellItem_Implementation(const FGFInventoryEntry& Entry, int32 NumItems)
{
	SellItem(Entry, NumItems);
}

void UInventoryComponent::ServerEnhancedItem_Implementation(const FGFInventoryEntry& InventoryEntry, int32 NumItems)
{
	EnhancedItem(InventoryEntry, NumItems);
}

void UInventoryComponent::ServerUnEquippedItem_Implementation(const FGFInventoryEntry& Entry, int32 NumItems)
{
	UnEquippedItem(Entry, NumItems);
}

void UInventoryComponent::ServerGetItemCount_Implementation(const FGameplayTag& ItemTag)
{
	GetItemCount(ItemTag);
}

void UInventoryComponent::ServerRemoveItem_Implementation(const FGameplayTag& ItemTag, int32 NumItems)
{
	RemoveItem(ItemTag, NumItems);
}


