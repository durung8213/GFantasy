// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/ColletionQuest.h"
#include "Inventory/InventoryComponent.h"

void UColletionQuest::InitializeFromTable(const FQuestDataTableRow& InRow)
{
    Super::InitializeFromTable(InRow);

    if (CachedInvenComp)
    {
        FMasterItemDefinition DefTarget = CachedInvenComp->GetItemDefinitionByTag(TargetTag);
        TargetName = DefTarget.ItemName;    // FText
        TargetIcon = DefTarget.Icon;        // UTexture2D*
    }
}

void UColletionQuest::OnQuestAccepted_Implementation()
{
	Super::OnQuestAccepted_Implementation();

	UE_LOG(LogTemp, Log, TEXT("CollectionQuest : OnQuestAccepted_Implementation"));


    if (CachedInvenComp)
    {
        // ���� ����Ʈ �������� ��ȭ�ҽ� ĳġ�ϴ� ��������Ʈ ����
        CachedInvenComp->OnItemCountChanged.AddDynamic(this, &UColletionQuest::HandleItemCountChanged);

        // ����Ʈ ������ �κ��丮�� Ȯ���ؼ� CurrentCount ����
        CurrentCount = CachedInvenComp->GetItemCount(TargetTag);
        SetCount(CurrentCount);
        UE_LOG(LogTemp, Log, TEXT("CollectionQuest : Initial Count - %d"), CurrentCount);
    }
}

void UColletionQuest::SetCount(int32 Amount)
{	
    UE_LOG(LogTemp, Log, TEXT("CollectionQuest : SetCount"));

	Super::SetCount(Amount);
}

void UColletionQuest::HandleItemCountChanged(const FGameplayTag& ItemTag, int32 NewCount)
{
    // ���� ���̰�, ��� �±װ� �¾ƾߡ�
    FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);

    if (QuestStateTag.MatchesTagExact(InProgressTag) && ItemTag == TargetTag)
    {
        //// ���� CurrentCount �� ������ ����
        //if (NewCount == CurrentCount)
        //{
        //    return;
        //}

        // ��ȭ�� ���� ���� ����
        CurrentCount = NewCount;
        SetCount(CurrentCount);
        // SetCount(0)�� ���ο��� CurrentCount += 0�� �ƴϵ���, 
        // SetCount �� ���밪 ���� �������� �ٲ�μ���.
        UE_LOG(LogTemp, Log, TEXT("QuestInstance: Count Changed => %d"), NewCount);
    }
}

