// QuestManagerComponent.cpp
#include "Quest/QuestManagerComponent.h"
#include "Quest/QuestDataTableRow.h"
#include "Quest/QuestInstance.h"
#include "Engine/DataTable.h"
#include "GFantasy/GFantasy.h"       // EQuestType 정의
#include "UI/NPC/NPCQuestListWidget.h"
#include "UI/Quest/QuestWidget.h"           // UQuestWidget 정의
#include "UI/Quest/QuestListWidget.h"
#include "UI/Quest/AvailableQuestWidget.h"
#include "UI/Quest/InProgressQuestWidget.h"
#include "UI/Quest/CompletedQuestWidget.h"
#include "UI/Quest/QuestMainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Controller/GASWorldPlayerController.h"
#include "Serialization/MemoryWriter.h"

UQuestManagerComponent::UQuestManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;  // Tick 불필요

    // 퀘스트 목록 클라로 보내기 위해 true켜기
    SetIsReplicatedByDefault(true);
    SetIsReplicated(true);
    bReplicateUsingRegisteredSubObjectList = true;

    CachedPlayerState = nullptr;
    CachedPlayerController = nullptr;

    TagLocked = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Locked"), false);
    TagAvailable = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Available"), false);
    TagInProgress = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);
    TagCompleted = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Completed"), false);    
}

bool UQuestManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    for (UQuestInstance* Q : AllQuestArr)
    {
        if (Q /* && Q->IsSupportedForNetworking()*/)
        {
            // 각 QuestInstance 객체를 서브오브젝트로 복제 등록
            WroteSomething |= Channel->ReplicateSubobject(Q, *Bunch, *RepFlags);
        }
    }
    return WroteSomething;
}

bool UQuestManagerComponent::IsReady() const
{
    return bInitialized;
}


void UQuestManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 서버→클라이언트 복제
    DOREPLIFETIME(UQuestManagerComponent, AllQuestArr);
    DOREPLIFETIME(UQuestManagerComponent, AvailableArr);
    DOREPLIFETIME(UQuestManagerComponent, InProgressArr);
    DOREPLIFETIME(UQuestManagerComponent, CompletedArr);
    DOREPLIFETIME_CONDITION(UQuestManagerComponent, bInitialized, COND_None);
}


/////////////////////////////////////////////////// 퀘스트 초기 세팅 //////////////////////////////////////////////////

void UQuestManagerComponent::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR   // 에디터나 개발 빌드에서만 실행
    LogSingleQuestSaveDataSize();
#endif
}

void UQuestManagerComponent::CachedOwnerReferences()
{
    AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(GetOwner());
    if (PS)
    {
        CachedPlayerState = PS;

        // PlayerState로부터 PlayerController 가져오기
        APlayerController* PC = PS->GetPlayerController();
        if (PC)
        {
            AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
            CachedPlayerController = MyPC;

            if (CachedPlayerController)
            {
                FindExistingQuestListWidget(AvailableArr, InProgressArr, CompletedArr);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: No PlayerController(Owner=%s)"), *PS->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManagerComponent: No PlayerState(%s)"), *GetOwner()->GetName());
    }
}

void UQuestManagerComponent::OnRep_Ready()
{
    if (bInitialized)
    {
        OnQuestManagerReady.Broadcast();
    }
}

void UQuestManagerComponent::LoadQuestsFromDataTable()
{
    if (QuestDataTable == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestManager : No QuestDataTable"));
        return;
    }

    static const FString ContextString(TEXT("LoadQuests"));

    for (const FName& RowName : QuestDataTable->GetRowNames())
    {
        const FQuestDataTableRow* Row = QuestDataTable->FindRow<FQuestDataTableRow>(RowName, ContextString);
        if (!Row) continue;

        // 타입에 따라 서브클래스 인스턴스 생성
        UQuestInstance* Quest = CreateQuestInstanceFromRow(*Row);
        if (Quest)
        {
            AllQuestArr.Add(Quest);

            //========= UQuestInstance 의 멤버 변수도 자동 복제 대상으로 등록 ==========
            // 이거 안하면 안됨. 생성하는 QuestInstance를 복제가능하다고 등록하는 것
            AddReplicatedSubObject(Quest);
            UE_LOG(LogTemp, Log, TEXT("Loaded Quest: %s (%s)"), *Quest->QuestName.ToString(), *Quest->GetClass()->GetName());
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Total Quests Loaded: %d"), AllQuestArr.Num());

    bInitialized = true;          // Replicate → OnRep_Ready() → 델리게이트
    OnQuestManagerReady.Broadcast();
}

void UQuestManagerComponent::SaveQuestData(TArray<FQuestSaveData>& SavedQuests) const
{
    SavedQuests.Empty();

    for (UQuestInstance* Quest : AllQuestArr)
    {
        if (!Quest) continue;

        FQuestSaveData QData;
        QData.S_QuestID = Quest->QuestID;
        QData.S_QuestStateTag = Quest->QuestStateTag;
        QData.S_CurrentCount = Quest->CurrentCount;
        QData.S_bCanAcceptQuest = Quest->bCanAcceptQuest;
        QData.S_bCanComplete = Quest->bCanComplete;

        SavedQuests.Add(QData);
    }
}

void UQuestManagerComponent::LoadQuestSaveData(const TArray<FQuestSaveData>& SavedQuests)
{
    if (!GetOwner()->HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadQuestData: SaveData - No Server"));
        return; // 서버 권한이 있을 때만
    }

    if (!QuestDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadQuestData: SaveData - No QuestDataTable"));
        return;
    }

    const FString ContextString(TEXT("LoadQuestData"));

    // (0) 이전에 등록된 서브오브젝트 해제
    for (UQuestInstance* OldQuest : AllQuestArr)
    {
        if (OldQuest)
        {
            RemoveReplicatedSubObject(OldQuest);
        }
    }

    // 모든 기존 인스턴스 초기화 또는 재분배
    AllQuestArr.Empty();

    for (const FName& RowName : QuestDataTable->GetRowNames())
    {
        // (1) ID에 해당하는 DataTableRow 또는 기존 인스턴스 찾기
        const FQuestDataTableRow* Row = QuestDataTable->FindRow<FQuestDataTableRow>(RowName, ContextString);        if (!Row) continue;

        if (!Row)
        {
            continue;
        }
        
        // (2) 인스턴스 생성 및 초기화
        UQuestInstance* Quest = CreateQuestInstanceFromRow(*Row);

        // (4) 리스트에 추가 & 복제 등록
        AllQuestArr.Add(Quest);
        //AddReplicatedSubObject(Quest);
    }
    
    UE_LOG(LogTemp, Log, TEXT("LoadQuestData: SaveData - Quest : %d"), AllQuestArr.Num());

    // 2) 저장된 진행 정보 덮어쓰기
    for (const FQuestSaveData& QData : SavedQuests)
    {
        // 저장된 ID와 일치하는 인스턴스 찾기
        UQuestInstance** FoundPtr = AllQuestArr.FindByPredicate(
            [&QData](UQuestInstance* Quest)
            {
                return Quest && Quest->QuestID == QData.S_QuestID;
            }
        );
        if (!FoundPtr || !*FoundPtr)
        {
            UE_LOG(LogTemp, Warning, TEXT("LoadQuestData: Only in SaveData %s"), *QData.S_QuestID.ToString());
            continue;
        }

        UQuestInstance* Quest = *FoundPtr;

        // 저장된 상태·카운트만 덮어쓰기
        Quest->SetQuestStateTag(QData.S_QuestStateTag);
        Quest->SetCount(QData.S_CurrentCount);
        Quest->bCanAcceptQuest = QData.S_bCanAcceptQuest;
        Quest->bCanComplete = QData.S_bCanComplete;
    }
    UE_LOG(LogTemp, Log, TEXT("LoadQuestData: SavedQuest %d Load Completed"), SavedQuests.Num());

    // 3) UI나 다른 시스템에 변경 알림
    OnRep_AllQuestArr();
}

void UQuestManagerComponent::PrepareForLoad(int32 TotalQuests)
{
    // 이전에 남아있던 데이터 초기화
    PendingQuestData.Empty();
    // 예상 개수만큼 메모리 확보
    PendingQuestData.Reserve(TotalQuests);

    ExpectedQuestCount = TotalQuests;

    bInitialized = false;

    UE_LOG(LogTemp, Log, TEXT("[Quest] PrepareForLoad: expecting %d quests"), TotalQuests);
}

void UQuestManagerComponent::LoadQuestPage(int32 PageIndex, const TArray<FQuestSaveData>& QuestPage)
{
    // 받은 페이지 데이터를 누적
    PendingQuestData.Append(QuestPage);

    UE_LOG(LogTemp, Log, TEXT("[Quest] Loaded page %d, current %d/%d"),
        PageIndex, PendingQuestData.Num(), ExpectedQuestCount);

    // 모든 페이지 로드 완료 시 최종 적용
    if (PendingQuestData.Num() >= ExpectedQuestCount)
    {
        FinalizeLoad();
    }
}

void UQuestManagerComponent::FinalizeLoad()
{
    const FString ContextString(TEXT("FinalizeLoad"));

    for (UQuestInstance* OldQuest : AllQuestArr)
    {
        if (OldQuest)
        {
            // Owner 에서 복제 대상 해제
            //GetOwner()->RemoveReplicatedSubObject(OldQuest);
        }
    }

    AllQuestArr.Empty();

    // 1) DataTable의 모든 RowName 순회
    for (const FName& RowName : QuestDataTable->GetRowNames())
    {
        // 2) 각 Row에 대응하는 데이터 가져오기
        const FQuestDataTableRow* DataRow = QuestDataTable->FindRow<FQuestDataTableRow>(RowName, ContextString);
        if (!DataRow)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Quest] FinalizeLoad: Missing DataRow '%s'"), *RowName.ToString());
            continue;
        }

        // 3) QuestInstance 생성
        UQuestInstance* Quest = CreateQuestInstanceFromRow(*DataRow);
        if (!Quest) continue;

        // 5) 배열에 추가 및 복제 등록
        AddReplicatedSubObject(Quest);
        AllQuestArr.Add(Quest);
    }

    // 2) PendingQuestData를 이용해 저장된 진행 정보 덮어쓰기
    for (const FQuestSaveData& Saved : PendingQuestData)
    {
        // ID가 FName이면 Saved.S_QuestID를, FString이면 FName(*Saved.S_QuestID) 사용
        UQuestInstance* const* Found = AllQuestArr.FindByPredicate(
            [&](UQuestInstance* Q)
            {
                return Q && Q->QuestID == Saved.S_QuestID;
            }
        );
        if (!Found || !*Found) continue;

        UQuestInstance* Quest = *Found;
        Quest->SetQuestStateTag(Saved.S_QuestStateTag);
        Quest->SetCount(Saved.S_CurrentCount);
        Quest->bCanAcceptQuest = Saved.S_bCanAcceptQuest;
        Quest->bCanComplete = Saved.S_bCanComplete;
    }

    // 이후 상태별 배열 분리, UI 갱신 등
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);
    OnRep_AllQuestArr();

    // 내부 초기화
    PendingQuestData.Empty();
    ExpectedQuestCount = 0;
    GetOwner()->ForceNetUpdate();

    bInitialized = true;
    OnQuestManagerReady.Broadcast();
}


UQuestInstance* UQuestManagerComponent::CreateQuestInstanceFromRow(const FQuestDataTableRow& Row)
{
    // 1) Row.D_QuestType 에 따라 클래스 선택
    TSubclassOf<UQuestInstance> Chosen = QuestInstanceClass;
    switch (Row.D_QuestType)
    {
    case EQuestType::Interaction:
        if (InteractionQuestClass) Chosen = InteractionQuestClass;
        break;
    case EQuestType::Collection:
        if (CollectionQuestClass) Chosen = CollectionQuestClass;
        break;
    case EQuestType::KillCount:
        if (KillCountQuestClass) Chosen = KillCountQuestClass;
        break;
    default:
        break;
    }

    // 2) NewObject 로 생성하고 초기화
    UQuestInstance* Inst = NewObject<UQuestInstance>(this, Chosen);
    if (Inst)
    {
        Inst->InitializeFromTable(Row);
    }
    return Inst;
}

void UQuestManagerComponent::OnRep_AllQuestArr()
{
    // 모든 배열이 들어오면 모든 퀘스트를 종류별로 나눠서 분리
    // 시작가능/진행중/완료된 퀘스트 나눠 보여주는 것    // 상태별로 분리
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);

    // 위젯에 전달
    GetWorld()->GetTimerManager().SetTimer(TH_QuestListWidget, this, &UQuestManagerComponent::SetupQuestUI, 2.0f, false);
    
    // 참조들 가져와서 여기서 위젯 배치
    CachedOwnerReferences();       
}


////////////////////////////////////////////////////////////////////////////////////////

TArray<UQuestInstance*> UQuestManagerComponent::GetQuestsByState(const FGameplayTag& StateTag) const
{
    TArray<UQuestInstance*> Filtered;
    for (UQuestInstance* Q : AllQuestArr)
    {
        if (Q && Q->QuestStateTag.MatchesTagExact(StateTag))
        {
            Filtered.Add(Q);
        }
    }
    return Filtered;
}

TArray<UQuestInstance*> UQuestManagerComponent::GetAvailableQuestsWithStartNPC(const FGameplayTag& StartNPCTag) const
{    
    TArray<UQuestInstance*> Filtered;

    for (UQuestInstance* Q : AllQuestArr)
    {
        if (Q
            && Q->QuestStateTag.MatchesTagExact(TagAvailable)/*QuestStateTag가 Available일 때*/
            && Q->QuestStartNPCTag == StartNPCTag)/*StartNPCTag가 InteractNPCTag일 때*/
        {
            Filtered.Add(Q);
        }
    }
    UE_LOG(LogTemp, Log, TEXT(
        "GetQuestsByStateAndStartTag - State: %s, StartTag: %s → Count: %d"),
        *TagAvailable.ToString(),
        *StartNPCTag.ToString(),
        Filtered.Num()
    );
    return Filtered;
}

TArray<UQuestInstance*> UQuestManagerComponent::GetInProgressQuestsWithEndNPC(const FGameplayTag& EndNPCTag) const
{    
    // UE_LOG(LogTemp, Log, TEXT("QuestManager : GetQuestsByStateAndEndTag: AllQuestArr.Num() = %d"), AllQuestArr.Num());
    TArray<UQuestInstance*> Filtered;

    for (UQuestInstance* Q : AllQuestArr)
    {
        if (Q
            && Q->QuestStateTag.MatchesTagExact(TagInProgress)/*InProgress 상태일 때*/
            && Q->QuestEndNPCTag == EndNPCTag)/*상호작용 NPC가 EndNPCTag일 때*/
        {
            Filtered.Add(Q);
        }
    }
    UE_LOG(LogTemp, Log, TEXT(
        "GetQuestsByStateAndEndTag - State: %s, EndTag: %s → Count: %d"),
        *TagInProgress.ToString(),
        *EndNPCTag.ToString(),
        Filtered.Num()
    );
    return Filtered;
}

TArray<UQuestInstance*> UQuestManagerComponent::GetInProgressQuestsWithBothNPC(const FGameplayTag& InTag) const
{
    // UE_LOG(LogTemp, Log, TEXT("QuestManager : GetQuestsByStateAndEndTag: AllQuestArr.Num() = %d"), AllQuestArr.Num());
    TArray<UQuestInstance*> Filtered;

    for (UQuestInstance* Q : AllQuestArr)
    {
        if (Q && Q->QuestStateTag.MatchesTagExact(TagInProgress)/*InProgress 상태일 때*/)
        {
            if (Q->QuestStartNPCTag == InTag || Q->QuestEndNPCTag == InTag)
            {
                Filtered.Add(Q);
            }
        }
    }
    UE_LOG(LogTemp, Log, TEXT(
        "GetInProgressQuestsWithBothNPC - State: %s, NPCTag: %s → Count: %d"),
        *TagInProgress.ToString(),
        *InTag.ToString(),
        Filtered.Num()
    );
    return Filtered;
}




//////////////////////////////////////////////////// 상호작용시 작동 ///////////////////////////////////////////////////

void UQuestManagerComponent::AcceptQuest(UQuestInstance* Q)
{
    if (!GetOwner()->HasAuthority()) return;

    // 퀘스트 수락했을때 일어나는 이벤트(상태 전환 등)
    Q->OnQuestAccepted(); // 상태 전환 함수

    // 전환된 퀘스트 상태에 따라 배열 변경
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);
}

void UQuestManagerComponent::ServerAcceptQuest_Implementation(UQuestInstance* Q)
{
    AcceptQuest(Q);
}

void UQuestManagerComponent::CompleteQuest(UQuestInstance* Q)
{
    if (!GetOwner()->HasAuthority()) return;
    // 퀘스트 완료시 실행될 이벤트
    UE_LOG(LogTemp, Log, TEXT("QuestManager : CompleteQuest"));

    // 퀘스트의 상태를 전환
    Q->OnQuestCompleted();

    // 전환된 퀘스트 상태에 따라 배열 변경
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);

    //// 수락된 퀘스트를 UI에 알려서 갱신하도록 델리게이트 발생
    //OnNPCQuestListUpdated.Broadcast();

    //// 서버 → 모든 클라이언트 Multicast 호출
    //Multicast_OnNPCQuestListUpdated();
}

void UQuestManagerComponent::ServerCompleteQuest_Implementation(UQuestInstance* Q)
{
    UE_LOG(LogTemp, Log, TEXT("QuestManager : ServerCompleteQuest"));
    CompleteQuest(Q);
}

//
//void UQuestManagerComponent::Multicast_OnNPCQuestListUpdated_Implementation()
//{
//    OnNPCQuestListUpdated.Broadcast();
//}


///////////////////////////////////////////////////// 위젯 생성 함수 /////////////////////////////////////////////////////////


void UQuestManagerComponent::SetQuestListWidgetInstance(UQuestListWidget* InWidget)
{
    QuestListWidgetInstance = InWidget;
}

void UQuestManagerComponent::SetupQuestUI()
{
    FindExistingQuestListWidget(AvailableArr, InProgressArr, CompletedArr);
}

void UQuestManagerComponent::SplitQuestsByState(const TArray<UQuestInstance*>& In, TArray<UQuestInstance*>& OutAvailable, TArray<UQuestInstance*>& OutInProgress, TArray<UQuestInstance*>& OutCompleted) const
{
    OutAvailable.Empty();    // Available 배열 비우기
    OutInProgress.Empty();   // InProgress 배열 비우기
    OutCompleted.Empty();    // Completed 배열 비우기

    for (UQuestInstance* Q : In)
    {
        if (!Q) continue;

        if (Q->QuestStateTag.MatchesTagExact(TagAvailable))
        {
            OutAvailable.Add(Q);
        }
        else if (Q->QuestStateTag.MatchesTagExact(TagInProgress))
        {
            OutInProgress.Add(Q);
        }
        else if (Q->QuestStateTag.MatchesTagExact(TagCompleted))
        {
            OutCompleted.Add(Q);
        }
    }
}


void UQuestManagerComponent::FindExistingQuestListWidget(const TArray<UQuestInstance*>& Available, const TArray<UQuestInstance*>& InProgress, const TArray<UQuestInstance*>& Completed)
{
    // 위젯 띄우기 PC를 이용하기
    if (CachedPlayerController)
    {
        CachedPlayerController->ClientShowQuestListWidget(Available, InProgress, Completed);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : No PC"));
    }
}




/////////////////////////////////////////////////// if Check 함수 /////////////////////////////////////////////////////

bool UQuestManagerComponent::CheckAcceptQuest(UQuestInstance* Q, FGameplayTag InNPCTag, FGameplayTag StartNPCTag)
{
    UE_LOG(LogTemp, Log, TEXT("QuestManager : CheckAccept %s"), *Q->QuestName.ToString());

    if (!Q)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : No QuestInstance"));
        return false;
    }

    if (Q->QuestStartNPCTag != InNPCTag)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : Not StartTag"));
        return false;
    }

    if (!Q->QuestStateTag.MatchesTagExact(StartNPCTag))
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : Not same State - Available"));
        return false;
    }

    return true;
}

bool UQuestManagerComponent::CheckCompleteQuest(UQuestInstance* Q, FGameplayTag InNPCTag, FGameplayTag EndNPCTag)
{
    UE_LOG(LogTemp, Log, TEXT("QuestManager : CheckComplete %s"), *Q->QuestName.ToString());
    UE_LOG(LogTemp, Log, TEXT("QuestManager : Check bCanComplete %s"), Q->bCanComplete ? TEXT("True") : TEXT("False"));
    if (!Q->bCanComplete)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : bCanComplete is false"));
        return false;
    }

    if (!Q)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : No QuestInstance"));
        return false;
    }

    if (Q->QuestEndNPCTag != InNPCTag)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : Not EndTag"));
        return false;
    }

    if (!Q->QuestStateTag.MatchesTagExact(EndNPCTag))
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : Not same State - InProgress"));
        return false;
    }

    return true;
}

void UQuestManagerComponent::CheckQuestData()
{
    if (QuestDataTable == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : No QuestDataTable"));
        return;
    }

    if (QuestInstanceClass == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : No QuestInctanceClass"));
        return;
    }
}

void UQuestManagerComponent::CheckQuestProgress(UQuestInstance* Q, FGameplayTag TargetTag, int32 DeltaCount)
{
    if (!Q)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : No QuestInctance"));
        return;
    }

    if (Q->TargetTag != TargetTag)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : Not TargetTag"));
        return;
    }

    if (!Q->bCanComplete)
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : bCanComplete is false"));
        return;
    }
}

void UQuestManagerComponent::LogSingleQuestSaveDataSize()
{
    // 1) 버퍼와 라이터 준비
    TArray<uint8> Buffer;
    FMemoryWriter Writer(Buffer, /* bIsPersistent= */ true);
    Writer.ArIsSaveGame = true;

    // 2) 샘플 데이터 세팅
    FQuestSaveData TestData;
    TestData.S_QuestID = FName(TEXT("TestQuest"));
    // 만약 S_QuestStatTag가 FGameplayTag 타입이라면
    TestData.S_QuestStateTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"));
    TestData.S_CurrentCount = 5;
    TestData.S_bCanAcceptQuest = true;
    TestData.S_bCanComplete = false;

    // 3) 멤버별로 직렬화
    Writer << TestData.S_QuestID;
    Writer << TestData.S_QuestStateTag;
    Writer << TestData.S_CurrentCount;
    Writer << TestData.S_bCanAcceptQuest;
    Writer << TestData.S_bCanComplete;

    // 4) 결과 출력
    UE_LOG(LogTemp, Log, TEXT("[Quest Debug] Single FQuestSaveData size = %d bytes"), Buffer.Num());
}