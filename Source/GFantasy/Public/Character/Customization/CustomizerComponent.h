// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomizeDefinition.h"
#include "GameplayTagContainer.h"
#include "CustomizeTable.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SaveGame/PlayerSaveData.h"
#include "CustomizerComponent.generated.h"

class UCustomizerComponent;

USTRUCT(BlueprintType)
struct FGFCustomizerEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag CustomizeTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FName SkeletalMeshName = FName();

	UPROPERTY(BlueprintReadOnly)
	FName CustomizeName = FName();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> CustomMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> SubCustomMesh = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface> CustomMaterial = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FLinearColor CustomMaterialColor = FLinearColor::Black;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FCustomizerChangedSignature, const FGFCustomizerEntry& /* 커스텀 메시 장착 */);
DECLARE_MULTICAST_DELEGATE_OneParam(FCustomizerRemovedSignature, const FGFCustomizerEntry& /* 커스텀 메시 삭제 */);

USTRUCT()
struct FGFCustomizerList : public FFastArraySerializer
{
	GENERATED_BODY()

	FGFCustomizerList() :
		OwnerComponent(nullptr) {}

	FGFCustomizerList(UCustomizerComponent* InComponent) :
		OwnerComponent(InComponent) {}

	void AddEntry(const FGameplayTag& CustomizeTag);
	void RemoveEntry(const FGFCustomizerEntry& RemoveEntry);
	void ChangeMaterial(const FGameplayTag& CustomizeTag, UMaterialInterface* Material, const FLinearColor& Color);

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGFCustomizerEntry, FGFCustomizerList>(Entries, DeltaParms, *this);
	}

	// 커스터마이징을 진짜로 로드하는 함수
	void LoadCustomizerListSaveData(const TArray<FCustomizerSaveData>& SavedCustoms);

	FCustomizerChangedSignature CustomizerChangedDelegate;
	FCustomizerRemovedSignature CustomizerRemovedDelegate;

private:

	friend class UCustomizerComponent;

	UPROPERTY()
	TArray<FGFCustomizerEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UCustomizerComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FGFCustomizerList> : TStructOpsTypeTraitsBase2<FGFCustomizerList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GFANTASY_API UCustomizerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(Replicated)
	FGFCustomizerList CustomizerList;

	UCustomizerComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ChangeMesh(const FGameplayTag& CustomizeTag);

	UFUNCTION(BlueprintCallable)
	void ChangeMaterial(const FGameplayTag& CustomizeTag, UMaterialInterface* Material, const FLinearColor& Color);

	UFUNCTION(BlueprintPure)
	FCustomizeDefinition GetCustomizeDefinition(const FGameplayTag& CustomizeTag) const;

	UFUNCTION(BlueprintCallable)
	TArray<FGFCustomizerEntry> GetCustomizerEntries();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Customize Definition")
	TObjectPtr<UCustomizeTable> CustomizeDefinition;

	UFUNCTION(Server, Reliable)
	void ServerChangeMesh(const FGameplayTag& CustomizeTag);

	UFUNCTION(Server, Reliable)
	void ServerChangeMaterial(const FGameplayTag& CustomizeTag, UMaterialInterface* Material, const FLinearColor& Color);

public:
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveCustomizerData(TArray<FCustomizerSaveData>& OutSaveData) const;

	// 페이징 로드를 시작할 때 총 인벤토리 아이템의 개수를 알려줌
	void PrepareForLoad(int32 TotalCustoms);

	// 페이지 단위로 넘어온 데이터를 받아 누적 로드
	void LoadCustomizerPage(int32 PageIndex, const TArray<FCustomizerSaveData>& CusPage);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadCustomizerSaveData(const TArray<FCustomizerSaveData>& SavedCustoms);

private:
	// --- 서버 RPC ---
	UFUNCTION(Server, Reliable)
	void Server_PrepareForLoad(int32 TotalCustoms);

	UFUNCTION(Server, Reliable)
	void Server_LoadCustomizerPage(int32 PageIndex, const TArray<FCustomizerSaveData>& CusPage);

	UFUNCTION(Server, Reliable)
	void Server_LoadCustomizerSaveData(const TArray<FCustomizerSaveData>& SavedCustoms);

private:
	// 페이징 중간 데이터를 저장할 임시 배열
	UPROPERTY()
	TArray<FCustomizerSaveData> PendingLoadData;

	// 총 로드해야 할 아이템 수
	UPROPERTY()
	int32 ExpectedCustoms = 0;

};
