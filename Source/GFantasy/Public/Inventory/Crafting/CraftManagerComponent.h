// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Interface/InventoryInterface.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SaveGame/PlayerSaveData.h"
#include "CraftManagerComponent.generated.h"

class UCraftManagerComponent;

USTRUCT(BlueprintType)
struct FGFCraftEntry : public FFastArraySerializerItem
{
	GENERATED_BODY();

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	FGameplayTag EntryTag = FGameplayTag();
	
	UPROPERTY(BlueprintType, BlueprintReadOnly)
	FGameplayTag CategoryTag = FGameplayTag();

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	FGameplayTag CraftedTag = FGameplayTag();

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	int32 CraftedAmount = 1;

	UPROPERTY(BlueprintType, BlueprintReadOnly)
	TArray<FCraftRecipeItemGroup> RecipeItems = TArray<FCraftRecipeItemGroup>();

private:
	
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCraftRecipeEntry, const FGFCraftEntry& /* 등록 레시피 */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCrafting, const FGFCraftEntry& /* 제작 */);

USTRUCT()
struct FGFCraftList : public FFastArraySerializer
{
	GENERATED_BODY();

	FGFCraftList() :
		OwnerComponent(nullptr) {}

	FGFCraftList(UActorComponent* InComponent) :
		OwnerComponent(InComponent) {}

	void GetInventoryComponent();

	// 인벤토리에서 레시피 제거후 크래프트에 레시피 등록
	void AddRecipe(const FGameplayTag& RecipeTag, const TArray<FCraftRecipeItemGroup>& RecipeItems);
	
	// 레시피에 따라 제작
	void CraftRecipe(const FGFCraftEntry& CraftEntry);

	// 제작법대로 제작해서 제작용 아이템이 소모됨
	void UseRecipeItem(const TArray<FCraftRecipeItemGroup>& RecipeItems);
	bool HasRecipe(const FGameplayTag& RecipeTag);
	bool HasAmountEnough(const TArray<FCraftRecipeItemGroup>& RecipeItems);

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGFCraftEntry, FGFCraftList>(Entries, DeltaParms, *this);
	}

	FOnCraftRecipeEntry CraftRecipeEntryDelegate;
	FOnCrafting CraftingDelegate;

private:

	friend class UCraftManagerComponent;
	
	UPROPERTY()
	TArray<FGFCraftEntry> Entries;
	
	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;
};

template<>
struct TStructOpsTypeTraits<FGFCraftList> : TStructOpsTypeTraitsBase2<FGFCraftList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GFANTASY_API UCraftManagerComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated)
	FGFCraftList CraftList;
	
	UCraftManagerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 제작법 등록
	UFUNCTION(BlueprintCallable)
	void AddRecipe(const FGameplayTag& RecipeTag, const TArray<FCraftRecipeItemGroup>& RecipeItems);

	// 제작하기
	UFUNCTION(BlueprintCallable)
	void CraftRecipe(const FGFCraftEntry& CraftEntry);

	UFUNCTION(BlueprintCallable)
	TArray<FGFCraftEntry> GetRecipeEntries();

	// 저장
	// 로드는 여기서 할 필요없고 레시피 등록이 됐는지만 알면됨
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveCraftData(TArray<FCraftSaveData>& OutSaveData) const;


private:

	UFUNCTION(Server, Reliable)
	void ServerAddRecipe(const FGameplayTag& RecipeTag, const TArray<FCraftRecipeItemGroup>& RecipeItems);

	UFUNCTION(Server, Reliable)
	void ServerCraftRecipe(const FGFCraftEntry& CraftEntry);
	

};
