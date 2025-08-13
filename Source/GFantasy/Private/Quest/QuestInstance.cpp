// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestInstance.h"
#include "Quest/QuestDataTableRow.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Quest/QuestManagerComponent.h"
#include "Inventory/InventoryComponent.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Controller/GASWorldPlayerController.h"
#include "Inventory/ItemTypes.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "GFQuest"

UQuestInstance::UQuestInstance()
{

}

void UQuestInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // UI에 표시하기 위해 복제할 것들 추가
    DOREPLIFETIME(UQuestInstance, QuestName);
    DOREPLIFETIME(UQuestInstance, QuestDescription);
    DOREPLIFETIME(UQuestInstance, QuestStateTag);
    DOREPLIFETIME(UQuestInstance, QuestStartNPCTag);
    DOREPLIFETIME(UQuestInstance, QuestEndNPCTag);
    DOREPLIFETIME(UQuestInstance, QuestID);
    DOREPLIFETIME(UQuestInstance, QuestType);
    DOREPLIFETIME(UQuestInstance, CurrentCount);
    DOREPLIFETIME(UQuestInstance, QuestProgress);
    DOREPLIFETIME(UQuestInstance, RequiredCount);
    DOREPLIFETIME(UQuestInstance, RewardItemTag);
    DOREPLIFETIME(UQuestInstance, RewardCount);
    DOREPLIFETIME(UQuestInstance, bCanAcceptQuest);
    DOREPLIFETIME(UQuestInstance, bCanComplete);
    //
    DOREPLIFETIME(UQuestInstance, TargetLocationTag);
    DOREPLIFETIME(UQuestInstance, NextQuestID);
    // UI표시를 위한 아이템 정보
    DOREPLIFETIME(UQuestInstance, TargetName);
    DOREPLIFETIME(UQuestInstance, RewardItemName);
    DOREPLIFETIME(UQuestInstance, TargetIcon);
    DOREPLIFETIME(UQuestInstance, RewardItemIcon);
}

void UQuestInstance::InitializeFromTable(const FQuestDataTableRow& InRow)
{
    bCanAcceptQuest = InRow.bCanAcceptQuest;
    QuestID = InRow.D_QuestID;
    NextQuestID = InRow.D_NextQuestID;
    QuestName = InRow.D_QuestName;
    QuestDescription = InRow.D_QuestDescription;

    QuestStartNPCTag = InRow.D_QuestStartNPCTag;
    QuestEndNPCTag = InRow.D_QuestEndNPCTag;
    QuestType = InRow.D_QuestType;
    QuestStateTag = InRow.D_QuestStateTag;
    PreviousStateTag = QuestStateTag;

    TargetTag = InRow.D_TargetTag;
    RequiredCount = InRow.D_RequiredCount;
    CurrentCount = InRow.D_CurrentCount;
    TargetLocationTag = InRow.D_TargetLocationTag;

    RewardItemTag = InRow.D_RewardItemTag;
    RewardCount = InRow.D_RewardCount;
    bCanComplete = false;

    // 인벤토리 가져오기
    CacheInventoryComponent();

    if (CachedInvenComp)
    {
        FMasterItemDefinition DefReward = CachedInvenComp->GetItemDefinitionByTag(RewardItemTag);
        RewardItemName = DefReward.ItemName;
        RewardItemIcon = DefReward.Icon;
    }
}

void UQuestInstance::OnQuestAccepted_Implementation()
{
    // 수락 시 InProgress 상태로 전환
    FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);
    SetQuestStateTag(InProgressTag);
}

void UQuestInstance::OnQuestCompleted_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("QuestInstance : OnQuestCompleted_Implementation"));
    
    if (!CachedInvenComp)
    {
        CacheInventoryComponent();
    }

    // 완료 시 Completed 상태로 전환
    FGameplayTag CompletedTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Completed"), false);
    SetQuestStateTag(CompletedTag);

    CachedInvenComp->RemoveItem(TargetTag, RequiredCount);

    // 보상 획득
    GetReward();

}

void UQuestInstance::OnRep_QuestStateTag()
{
    UQuestManagerComponent* Manager = Cast<UQuestManagerComponent>(GetOuter());
    if (!Manager)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : OnRep_QuestStateTag - No QuestManager"));
        return;
    }
    AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(Manager->GetOwner());
    if (!PS)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : OnRep_QuestStateTag - No PS"));
        return;
    }
    APlayerController* PC = PS->GetPlayerController();
    if (PC)
    {
        AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
        MyPC->ServerShowQuestListWidget();

    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : OnRep_QuestStateTag - No PC"));
        return;
    }

    FGameplayTag PrevState = PreviousStateTag;
    OnQuestStateChanged.Broadcast(this, PrevState);
    PreviousStateTag = QuestStateTag;

    // 수락된 퀘스트를 UI에 알려서 NPC의 퀘스트 리스트를 갱신하도록 델리게이트 발생
    OnNPCQuestListUpdated.Broadcast();
}

void UQuestInstance::OnRep_bCanComplete()
{
    FGameplayTag PrevState = PreviousStateTag;
    OnQuestStateChanged.Broadcast(this, PrevState);
    PreviousStateTag = QuestStateTag;
}

void UQuestInstance::OnRep_CurrentCount()
{
    // CurrentCount가 변했을때 위젯변화시킬함수
    UE_LOG(LogTemp, Log, TEXT("QuestInstance : OnRep_CurrentCount"));

    UQuestManagerComponent* Manager = Cast<UQuestManagerComponent>(GetOuter());
    if (!Manager)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : No QuestManager"));
        return;
    }
    AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(Manager->GetOwner());
    if (!PS)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : No PS"));
        return;
    }
    APlayerController* PC = PS->GetPlayerController();
    if (PC)
    {
        AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
        MyPC->ServerShowQuestListWidget();
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : No PC"));
        return;
    }
}

void UQuestInstance::SetQuestStateTag(const FGameplayTag& NewStateTag)
{
    if (QuestStateTag == NewStateTag)
    {
        return;
    }    
    
    UQuestManagerComponent* OwnerComp = Cast<UQuestManagerComponent>(GetOuter());
    bool bIsServer = false;
    if (OwnerComp && OwnerComp->GetOwner())
    {
        bIsServer = OwnerComp->GetOwner()->HasAuthority();
    }

    // 2) NetMode 로깅
    UWorld* World = GetWorld();
    const ENetMode NetMode = World ? World->GetNetMode() : NM_Standalone;

    UE_LOG(LogTemp, Log, TEXT("[SetQuestStateTag] Called on %s (NetMode=%d)"),
        bIsServer ? TEXT("Server") : TEXT("Client"),
        (int32)NetMode);


    UE_LOG(LogTemp, Log, TEXT("QusetInstance : PreviousStateTag = %s"), *QuestStateTag.ToString());
    UE_LOG(LogTemp, Log, TEXT("QusetInstance : SetQuestStateTag = %s"), *NewStateTag.ToString());

    // 여기서 태그 상태 변화
    // 서버에서만 태그를 갱신하고 복제 트리거
    // UObject니까 World를 통해 권한 체크
    //UQuestManagerComponent* OwnerComp = Cast<UQuestManagerComponent>(GetOuter());
    
    if (GetOuter()->GetWorld()->GetAuthGameMode())
    {
        //FGameplayTag PrevState = PreviousStateTag;
        QuestStateTag = NewStateTag;
        //OnQuestStateChanged.Broadcast(this, PrevState);
        UE_LOG(LogTemp, Log, TEXT("QusetInstance : CurrnetStateTag = %s"), *QuestStateTag.ToString());
    }
}


void UQuestInstance::SetCount(int32 Amount)
{
    UE_LOG(LogTemp, Log, TEXT("QuestInstance : SetCount"));

    // 현재 수량 증가
    CurrentCount = FMath::Clamp(Amount, 0, RequiredCount);

    OnCountChanged.Broadcast(this);

    // 진행률 계산
    UpdateProgress();

    //// 현재 수량 증가
    //int32 Clamped = FMath::Clamp(Amount, 0, RequiredCount);
    //if (CurrentCount != Clamped)
    //{
    //    CurrentCount = Clamped;
    //    OnCountChanged.Broadcast(this);

    //    // 진행률 계산
    //    UpdateProgress();
    //}
}

void UQuestInstance::UpdateProgress()
{
    UE_LOG(LogTemp, Log, TEXT("QuestInstance : UpdateProgress"));

    if(RequiredCount > 0)
    {
        // 정수 나눗셈이 아닌 실수 계산으로 변경
        const float Ratio = static_cast<float>(CurrentCount) / static_cast<float>(RequiredCount);
        const float PercentF = Ratio * 100.0f;

        // 소수점 올림·내림 또는 반올림 중 선택
        QuestProgress = FMath::Clamp(FMath::RoundToInt(PercentF), 0, 100);
    }
    else
    {
        QuestProgress = 0;
    }
    SetbCanComplete(QuestProgress);
}

void UQuestInstance::SetbCanComplete(int32 Progress)
{
    // 진행률로 진행가능 체크
    if (Progress >= 100)
    {
        bCanComplete = true;
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : QuestProgress %d percent"), Progress);
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : Can completable! (%s)"), *QuestName.ToString());
    }
    else
    {
        bCanComplete = false;
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : QuestProgress %d percent"), Progress);
        UE_LOG(LogTemp, Log, TEXT("QuestInstance : Can't completable! (%s)"), *QuestName.ToString());
    }
}

int32 UQuestInstance::GetProgressPercent() const
{
    return QuestProgress;
}

void UQuestInstance::GetReward()
{
    if (!CachedInvenComp)
    {
        CacheInventoryComponent();
    }
    
    CachedInvenComp->AddItem(RewardItemTag, RewardCount);
}

int32 UQuestInstance::GetRewardCount()
{
    return RewardCount;
}

FText UQuestInstance::ChangeStateTagToText(const FGameplayTag& InTag) const
{
    if (!InTag.IsValid())
    {
        return FText::GetEmpty();
    }

    FName TagName = InTag.GetTagName();
    static const FGameplayTag AvailableTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Available"), false);
    static const FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);
    static const FGameplayTag CompletedTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Completed"), false);
    FString FullString = TagName.ToString();

    if (InTag == AvailableTag)
    {
        return FText::FromString(TEXT("시작 가능"));
    }
    else if (InTag == InProgressTag)
    {
        if (!bCanComplete)
        {
            return FText::FromString(TEXT("진행중"));
        }
        else
        {
            return FText::FromString(TEXT("완료 가능"));
        }
    }
    else if (InTag == CompletedTag)
    {
        return FText::FromString(TEXT("완료"));
    }

    return FText::FromString(FullString);
}

FText UQuestInstance::ChangeTagToText(const FGameplayTag& InTag)
{
    if (!InTag.IsValid())
    {
        return FText::GetEmpty();
    }

    static const TMap<FString, FText> TailToKorean =
    {
        { TEXT("Deer"),         LOCTEXT("Deer_KR",      "사슴") },
        { TEXT("Wolf"),         LOCTEXT("Wolf_KR",      "늑대") },
        { TEXT("WolfMan"),      LOCTEXT("WolfMan_KR",   "산적") },
        { TEXT("DarkNight"),    LOCTEXT("DarkNight_KR", "망령기사") },
        { TEXT("Succubus"),     LOCTEXT("Succubus_KR",  "서큐버스") },

        { TEXT("HyangHee"),     LOCTEXT("HyangHees_KR",     "향희") },
        { TEXT("LeadActor"),    LOCTEXT("LeadActor_KR",     "서주연") },
        { TEXT("BigFineTree"),  LOCTEXT("BigFineTree_KR",   "큰솔") },
        { TEXT("BigBro"),       LOCTEXT("BigBro_KR",        "빅브로") },
        { TEXT("MaxBro"),       LOCTEXT("MaxBro_KR",        "맥스브로") },
        { TEXT("SmallBro"),     LOCTEXT("SmallBro_KR",      "민브로") },
        
    };

    const FString Full = InTag.GetTagName().ToString();
    FString Tail = Full;

    int32 LastDot = INDEX_NONE;
    if (Full.FindLastChar('.', LastDot))
    {
        Tail = Full.Mid(LastDot + 1);
    }

    /*-----------------------------------------------------------------------
     * 3) 매핑 시도 → 성공하면 한글, 실패하면 영문 그대로
     *---------------------------------------------------------------------*/
    if (const FText* Found = TailToKorean.Find(Tail))
    {
        return *Found;
    }

    return FText::FromString(Tail);

    //// "Quest.State.Available" 같은 전체 태그를 문자열로 얻기
    //FString Full = InTag.GetTagName().ToString();

    //// 마지막 점 위치 찾기
    //int32 LastDot = INDEX_NONE;
    //if (Full.FindLastChar('.', LastDot))
    //{
    //    // 점 다음부터 끝까지 추출
    //    return FText::FromString(Full.Mid(LastDot + 1));
    //}

    //// 점이 없다면 전체 문자열을 반환
    //return FText::FromString(Full);
}
#undef LOCTEXT_NAMESPACE


void UQuestInstance::CacheInventoryComponent()
{
    // 인벤토리 컴포넌트 찾아서 캐시
    if (!CachedInvenComp)
    {
        UQuestManagerComponent* Manager = Cast<UQuestManagerComponent>(GetOuter());
        if (!Manager)
        {
            UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No QuestManager"));
            return;
        }
        AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(Manager->GetOwner());
        if (!PS)
        {
            UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No PS"));
            return;
        }
        APlayerController* PC = PS->GetPlayerController();
        if (!PC)
        {
            UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No PC"));
            return;
        }
        AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
        if (!MyPC)
        {
            UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No MyPC"));
            return;
        }
        UInventoryComponent* InvenComp = MyPC->Execute_GetInventoryComponent(MyPC);
        if (!InvenComp)
        {
            UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No QuestManager"));
            return;
        }

        //if (CheckInventory(Manager, PS, PC, InvenComp))
        //{
        //    return;
        //}

        CachedInvenComp = InvenComp;
        UE_LOG(LogTemp, Log, TEXT("CollectionQuest: Cached InventoryComponent -> %s"), *CachedInvenComp->GetName());
    }
}

bool UQuestInstance::CheckInventory(UQuestManagerComponent* QM, AGASWorldPlayerState* PS, APlayerController* PC, UInventoryComponent* InvenComp)
{
    bool bAllValid = true;

    if (!QM)
    {
        UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No QuestManager"));
        bAllValid = false;
    }

    if (!PS)
    {
        UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No PlayerState"));
        bAllValid = false;
    }

    if (!PC)
    {
        UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No PlayeController"));
        bAllValid = false;
    }

    if (!InvenComp)
    {
        UE_LOG(LogTemp, Log, TEXT("CollectionQuest : No InventoryComponent"));
        bAllValid = false;
    }

    return bAllValid;
}