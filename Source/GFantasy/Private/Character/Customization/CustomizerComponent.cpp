// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Customization/CustomizerComponent.h"

#include "Libraries/GASAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"


void FGFCustomizerList::AddEntry(const FGameplayTag& CustomizeTag)
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const FCustomizeDefinition Customize = OwnerComponent->GetCustomizeDefinition(CustomizeTag);

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFCustomizerEntry& Entry = *EntryIt;

		if (Entry.SkeletalMeshName == Customize.SkeletalMeshName)
		{
			RemoveEntry(Entry);
			break;
		}
	}	
		
	FGFCustomizerEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.CustomizeTag = CustomizeTag;
	NewEntry.SkeletalMeshName = Customize.SkeletalMeshName;
	NewEntry.CustomizeName = Customize.CustomizeName;
	NewEntry.CustomMesh = Customize.CustomMesh;
	NewEntry.SubCustomMesh = Customize.SubCustomMesh;	
	NewEntry.CustomMaterial = Customize.CustomMaterial;
	NewEntry.CustomMaterialColor = Customize.CustomMaterialColor;

	CustomizerChangedDelegate.Broadcast(NewEntry);

	MarkItemDirty(NewEntry);	
}

void FGFCustomizerList::RemoveEntry(const FGFCustomizerEntry& RemoveEntry)
{
	check(OwnerComponent);

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFCustomizerEntry& Entry = *EntryIt;
		
		if (Entry.SkeletalMeshName == RemoveEntry.SkeletalMeshName)
		{			
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FGFCustomizerList::ChangeMaterial(const FGameplayTag& CustomizeTag, UMaterialInterface* Material, const FLinearColor& Color)
{
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGFCustomizerEntry& Entry = *EntryIt;

		if (Entry.CustomizeTag.MatchesTag(CustomizeTag))
		{
			Entry.CustomMaterial = Material;
			Entry.CustomMaterialColor = Color;
			MarkItemDirty(Entry);
			CustomizerChangedDelegate.Broadcast(Entry);
			break;
		}
	}
}

void FGFCustomizerList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	// ���� �߰�
	if (!OwnerComponent) return;

	for (const int32 Index : RemovedIndices)
	{
		const FGFCustomizerEntry& Entry = Entries[Index];

		CustomizerRemovedDelegate.Broadcast(Entry);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CustomizeEntry Remove"));
	}
}

void FGFCustomizerList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	// ���� �߰�
	if (!OwnerComponent) return;

	for (const int32 Index : AddedIndices)
	{
		FGFCustomizerEntry& Entry = Entries[Index];

		CustomizerChangedDelegate.Broadcast(Entry);
	}
}

void FGFCustomizerList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{	
	// ���� �߰�
	if (!OwnerComponent) return;

	for (const int32 Index : ChangedIndices)
	{
		FGFCustomizerEntry& Entry = Entries[Index];

		CustomizerChangedDelegate.Broadcast(Entry);
	}
}

void FGFCustomizerList::LoadCustomizerListSaveData(const TArray<FCustomizerSaveData>& SavedCustoms)
{
	UE_LOG(LogTemp, Warning, TEXT("[CustomizerList] Loading %d entries"), SavedCustoms.Num());

	// 1) ����� �±� ����Ʈ�� ����
	TArray<FGameplayTag> SavedTags;
	SavedTags.Reserve(SavedCustoms.Num());
	for (const FCustomizerSaveData& Data : SavedCustoms)
	{
		SavedTags.AddUnique(Data.CustomizeTag);
	}

	// 2) ���� Entries �߿� ����� �±׿� ���� �� ����
	{
		// ����: RemoveEntry ���ο��� MarkArrayDirty()�� ȣ��ǹǷ�
		// �ݺ� �� Entries�� ����� �� �ֽ��ϴ�.
		for (int32 i = Entries.Num() - 1; i >= 0; --i)
		{
			const FGFCustomizerEntry& Entry = Entries[i];
			if (!SavedTags.Contains(Entry.CustomizeTag))
			{
				RemoveEntry(Entry);
			}
		}
	}

	// 3) ����� ������ �������, ������ AddEntry, ������ ��Ƽ����/���� ������Ʈ
	for (const FCustomizerSaveData& Data : SavedCustoms)
	{
		// �̹� �����ϴ��� Ȯ��
		bool bExists = false;
		for (const FGFCustomizerEntry& Entry : Entries)
		{
			if (Entry.CustomizeTag == Data.CustomizeTag)
			{
				bExists = true;
				break;
			}
		}

		if (!bExists)
		{
			// ���ǵ� Default ������ �ű� Entry ���� (�޽�/�̸� ��)
			AddEntry(Data.CustomizeTag);
		}

		// AddEntry ���� ��Ʈ���� �ݵ�� �ϳ��� �����ϹǷ�, ChangeMaterial �� ��Ƽ���� & ���� ����
		ChangeMaterial(Data.CustomizeTag, Data.CustomMaterial.Get(), Data.CustomMaterialColor);
	}
}

UCustomizerComponent::UCustomizerComponent() :
	CustomizerList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	//SetIsReplicatedByDefault(true);

}

void UCustomizerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCustomizerComponent, CustomizerList);
}

void UCustomizerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCustomizerComponent::ChangeMesh(const FGameplayTag& CustomizeTag)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerChangeMesh(CustomizeTag);
		return;
	}

	CustomizerList.AddEntry(CustomizeTag);
}

void UCustomizerComponent::ChangeMaterial(const FGameplayTag& CustomizeTag, UMaterialInterface* Material, const FLinearColor& Color)
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return;

	if (!Owner->HasAuthority())
	{
		ServerChangeMaterial(CustomizeTag, Material, Color);
		return;
	}

	CustomizerList.ChangeMaterial(CustomizeTag, Material, Color);
}

FCustomizeDefinition UCustomizerComponent::GetCustomizeDefinition(const FGameplayTag& CustomizeTag) const
{
	checkf(CustomizeDefinition, TEXT("No Customize Def Inside Comp &s"), *GetNameSafe(this));

	for (const auto& Pair : CustomizeDefinition->TagToTables)
	{
		if (CustomizeTag.MatchesTag(Pair.Key))
		{			
			if (const FCustomizeDefinition* ValidCustomize = UGASAbilitySystemLibrary::GetDataTableRowByTag<FCustomizeDefinition>(Pair.Value, CustomizeTag))
			{
				return *ValidCustomize;
			}
		}
	}

	return FCustomizeDefinition();
}

TArray<FGFCustomizerEntry> UCustomizerComponent::GetCustomizerEntries()
{
	return CustomizerList.Entries;
}


void UCustomizerComponent::ServerChangeMaterial_Implementation(const FGameplayTag& CustomizeTag, UMaterialInterface* Material, const FLinearColor& Color)
{
	ChangeMaterial(CustomizeTag, Material, Color);
}

void UCustomizerComponent::ServerChangeMesh_Implementation(const FGameplayTag& CustomizeTag)
{
	ChangeMesh(CustomizeTag);
}

void UCustomizerComponent::SaveCustomizerData(TArray<FCustomizerSaveData>& OutSaveData) const
{
	OutSaveData.Empty();
	for (const auto& Entry : CustomizerList.Entries)
	{
		FCustomizerSaveData Data;
		Data.CustomizeTag = Entry.CustomizeTag;
		Data.SkeletalMeshName = Entry.SkeletalMeshName;
		Data.CustomizeName = Entry.CustomizeName;
		Data.CustomMesh = Entry.CustomMesh;
		Data.SubCustomMesh = Entry.SubCustomMesh;
		Data.CustomMaterial = Entry.CustomMaterial;
		Data.CustomMaterialColor = Entry.CustomMaterialColor;
		OutSaveData.Add(Data);
	}
}

void UCustomizerComponent::PrepareForLoad(int32 TotalCustoms)
{	
	// Ŭ���̾�Ʈ�� ���: ������ RPC ��û
	if (!GetOwner()->HasAuthority())
	{
		Server_PrepareForLoad(TotalCustoms);
		return;
	}

	// ����¡ �ӽ� ����� �ʱ�ȭ
	PendingLoadData.Empty();

	ExpectedCustoms = TotalCustoms;
}

void UCustomizerComponent::LoadCustomizerPage(int32 PageIndex, const TArray<FCustomizerSaveData>& CusPage)
{	
	// Ŭ���̾�Ʈ�� ���: ������ RPC ��û
	if (!GetOwner()->HasAuthority())
	{
		Server_LoadCustomizerPage(PageIndex, CusPage);
		return;
	}

	// ���� ������ �����͸� �ӽ� �迭�� ����
	PendingLoadData.Append(CusPage);

	// ������ ���������� ��� ���������� ���� �ε� ����
	if (PendingLoadData.Num() >= ExpectedCustoms)
	{
		LoadCustomizerSaveData(PendingLoadData);
		// Ŭ������ �θ�, ���� ���ǿ� �ٽ� �ε��ص� �����մϴ�.
		PendingLoadData.Empty();
	}
}

void UCustomizerComponent::LoadCustomizerSaveData(const TArray<FCustomizerSaveData>& SavedCustoms)
{	
	// Ŭ���̾�Ʈ�� ���: ������ RPC ��û
	if (!GetOwner()->HasAuthority())
	{
		Server_LoadCustomizerSaveData(SavedCustoms);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Customizer] LoadCustomizerSaveData called with %d items"), SavedCustoms.Num());
	for (const auto& Data : SavedCustoms)
	{
		UE_LOG(LogTemp, Warning, TEXT("  �� Tag: %s, Mesh: %s"), *Data.CustomizeTag.ToString(), *Data.SkeletalMeshName.ToString());
	}

	// **���⼭�� ���� �� �� �ٸ�!**
	CustomizerList.LoadCustomizerListSaveData(SavedCustoms);

	// �������� �ٷ� ���� ����
	PendingLoadData = SavedCustoms;
	ExpectedCustoms = 0;
}

void UCustomizerComponent::Server_PrepareForLoad_Implementation(int32 TotalCustoms)
{
	PrepareForLoad(TotalCustoms);
}

void UCustomizerComponent::Server_LoadCustomizerPage_Implementation(int32 PageIndex, const TArray<FCustomizerSaveData>& CusPage)
{
	LoadCustomizerPage(PageIndex, CusPage);
}

void UCustomizerComponent::Server_LoadCustomizerSaveData_Implementation(const TArray<FCustomizerSaveData>& SavedCustoms)
{
	LoadCustomizerSaveData(SavedCustoms);
}
