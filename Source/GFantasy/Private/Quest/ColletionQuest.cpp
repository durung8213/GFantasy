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
        // 이후 퀘스트 아이템이 변화할시 캐치하는 델리게이트 연결
        CachedInvenComp->OnItemCountChanged.AddDynamic(this, &UColletionQuest::HandleItemCountChanged);

        // 퀘스트 수락시 인벤토리를 확인해서 CurrentCount 설정
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
    // 진행 중이고, 대상 태그가 맞아야…
    FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);

    if (QuestStateTag.MatchesTagExact(InProgressTag) && ItemTag == TargetTag)
    {
        //// 이전 CurrentCount 와 같으면 무시
        //if (NewCount == CurrentCount)
        //{
        //    return;
        //}

        // 변화가 있을 때만 갱신
        CurrentCount = NewCount;
        SetCount(CurrentCount);
        // SetCount(0)이 내부에서 CurrentCount += 0이 아니도록, 
        // SetCount 로 절대값 세팅 로직으로 바꿔두세요.
        UE_LOG(LogTemp, Log, TEXT("QuestInstance: Count Changed => %d"), NewCount);
    }
}

