// QuestManagerComponent.cpp
#include "Quest/QuestManagerComponent.h"
#include "Quest/QuestDataTableRow.h"
#include "Quest/QuestInstance.h"
#include "Engine/DataTable.h"
#include "GFantasy/GFantasy.h"       // EQuestType ����
#include "UI/NPC/NPCQuestListWidget.h"
#include "UI/Quest/QuestWidget.h"           // UQuestWidget ����
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
    PrimaryComponentTick.bCanEverTick = false;  // Tick ���ʿ�

    // ����Ʈ ��� Ŭ��� ������ ���� true�ѱ�
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
            // �� QuestInstance ��ü�� ���������Ʈ�� ���� ���
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

    // ������Ŭ���̾�Ʈ ����
    DOREPLIFETIME(UQuestManagerComponent, AllQuestArr);
    DOREPLIFETIME(UQuestManagerComponent, AvailableArr);
    DOREPLIFETIME(UQuestManagerComponent, InProgressArr);
    DOREPLIFETIME(UQuestManagerComponent, CompletedArr);
    DOREPLIFETIME_CONDITION(UQuestManagerComponent, bInitialized, COND_None);
}


/////////////////////////////////////////////////// ����Ʈ �ʱ� ���� //////////////////////////////////////////////////

void UQuestManagerComponent::BeginPlay()
{
    Super::BeginPlay();

#if WITH_EDITOR   // �����ͳ� ���� ���忡���� ����
    LogSingleQuestSaveDataSize();
#endif
}

void UQuestManagerComponent::CachedOwnerReferences()
{
    AGASWorldPlayerState* PS = Cast<AGASWorldPlayerState>(GetOwner());
    if (PS)
    {
        CachedPlayerState = PS;

        // PlayerState�κ��� PlayerController ��������
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

        // Ÿ�Կ� ���� ����Ŭ���� �ν��Ͻ� ����
        UQuestInstance* Quest = CreateQuestInstanceFromRow(*Row);
        if (Quest)
        {
            AllQuestArr.Add(Quest);

            //========= UQuestInstance �� ��� ������ �ڵ� ���� ������� ��� ==========
            // �̰� ���ϸ� �ȵ�. �����ϴ� QuestInstance�� ���������ϴٰ� ����ϴ� ��
            AddReplicatedSubObject(Quest);
            UE_LOG(LogTemp, Log, TEXT("Loaded Quest: %s (%s)"), *Quest->QuestName.ToString(), *Quest->GetClass()->GetName());
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Total Quests Loaded: %d"), AllQuestArr.Num());

    bInitialized = true;          // Replicate �� OnRep_Ready() �� ��������Ʈ
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
        return; // ���� ������ ���� ����
    }

    if (!QuestDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("LoadQuestData: SaveData - No QuestDataTable"));
        return;
    }

    const FString ContextString(TEXT("LoadQuestData"));

    // (0) ������ ��ϵ� ���������Ʈ ����
    for (UQuestInstance* OldQuest : AllQuestArr)
    {
        if (OldQuest)
        {
            RemoveReplicatedSubObject(OldQuest);
        }
    }

    // ��� ���� �ν��Ͻ� �ʱ�ȭ �Ǵ� ��й�
    AllQuestArr.Empty();

    for (const FName& RowName : QuestDataTable->GetRowNames())
    {
        // (1) ID�� �ش��ϴ� DataTableRow �Ǵ� ���� �ν��Ͻ� ã��
        const FQuestDataTableRow* Row = QuestDataTable->FindRow<FQuestDataTableRow>(RowName, ContextString);        if (!Row) continue;

        if (!Row)
        {
            continue;
        }
        
        // (2) �ν��Ͻ� ���� �� �ʱ�ȭ
        UQuestInstance* Quest = CreateQuestInstanceFromRow(*Row);

        // (4) ����Ʈ�� �߰� & ���� ���
        AllQuestArr.Add(Quest);
        //AddReplicatedSubObject(Quest);
    }
    
    UE_LOG(LogTemp, Log, TEXT("LoadQuestData: SaveData - Quest : %d"), AllQuestArr.Num());

    // 2) ����� ���� ���� �����
    for (const FQuestSaveData& QData : SavedQuests)
    {
        // ����� ID�� ��ġ�ϴ� �ν��Ͻ� ã��
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

        // ����� ���¡�ī��Ʈ�� �����
        Quest->SetQuestStateTag(QData.S_QuestStateTag);
        Quest->SetCount(QData.S_CurrentCount);
        Quest->bCanAcceptQuest = QData.S_bCanAcceptQuest;
        Quest->bCanComplete = QData.S_bCanComplete;
    }
    UE_LOG(LogTemp, Log, TEXT("LoadQuestData: SavedQuest %d Load Completed"), SavedQuests.Num());

    // 3) UI�� �ٸ� �ý��ۿ� ���� �˸�
    OnRep_AllQuestArr();
}

void UQuestManagerComponent::PrepareForLoad(int32 TotalQuests)
{
    // ������ �����ִ� ������ �ʱ�ȭ
    PendingQuestData.Empty();
    // ���� ������ŭ �޸� Ȯ��
    PendingQuestData.Reserve(TotalQuests);

    ExpectedQuestCount = TotalQuests;

    bInitialized = false;

    UE_LOG(LogTemp, Log, TEXT("[Quest] PrepareForLoad: expecting %d quests"), TotalQuests);
}

void UQuestManagerComponent::LoadQuestPage(int32 PageIndex, const TArray<FQuestSaveData>& QuestPage)
{
    // ���� ������ �����͸� ����
    PendingQuestData.Append(QuestPage);

    UE_LOG(LogTemp, Log, TEXT("[Quest] Loaded page %d, current %d/%d"),
        PageIndex, PendingQuestData.Num(), ExpectedQuestCount);

    // ��� ������ �ε� �Ϸ� �� ���� ����
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
            // Owner ���� ���� ��� ����
            //GetOwner()->RemoveReplicatedSubObject(OldQuest);
        }
    }

    AllQuestArr.Empty();

    // 1) DataTable�� ��� RowName ��ȸ
    for (const FName& RowName : QuestDataTable->GetRowNames())
    {
        // 2) �� Row�� �����ϴ� ������ ��������
        const FQuestDataTableRow* DataRow = QuestDataTable->FindRow<FQuestDataTableRow>(RowName, ContextString);
        if (!DataRow)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Quest] FinalizeLoad: Missing DataRow '%s'"), *RowName.ToString());
            continue;
        }

        // 3) QuestInstance ����
        UQuestInstance* Quest = CreateQuestInstanceFromRow(*DataRow);
        if (!Quest) continue;

        // 5) �迭�� �߰� �� ���� ���
        AddReplicatedSubObject(Quest);
        AllQuestArr.Add(Quest);
    }

    // 2) PendingQuestData�� �̿��� ����� ���� ���� �����
    for (const FQuestSaveData& Saved : PendingQuestData)
    {
        // ID�� FName�̸� Saved.S_QuestID��, FString�̸� FName(*Saved.S_QuestID) ���
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

    // ���� ���º� �迭 �и�, UI ���� ��
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);
    OnRep_AllQuestArr();

    // ���� �ʱ�ȭ
    PendingQuestData.Empty();
    ExpectedQuestCount = 0;
    GetOwner()->ForceNetUpdate();

    bInitialized = true;
    OnQuestManagerReady.Broadcast();
}


UQuestInstance* UQuestManagerComponent::CreateQuestInstanceFromRow(const FQuestDataTableRow& Row)
{
    // 1) Row.D_QuestType �� ���� Ŭ���� ����
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

    // 2) NewObject �� �����ϰ� �ʱ�ȭ
    UQuestInstance* Inst = NewObject<UQuestInstance>(this, Chosen);
    if (Inst)
    {
        Inst->InitializeFromTable(Row);
    }
    return Inst;
}

void UQuestManagerComponent::OnRep_AllQuestArr()
{
    // ��� �迭�� ������ ��� ����Ʈ�� �������� ������ �и�
    // ���۰���/������/�Ϸ�� ����Ʈ ���� �����ִ� ��    // ���º��� �и�
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);

    // ������ ����
    GetWorld()->GetTimerManager().SetTimer(TH_QuestListWidget, this, &UQuestManagerComponent::SetupQuestUI, 2.0f, false);
    
    // ������ �����ͼ� ���⼭ ���� ��ġ
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
            && Q->QuestStateTag.MatchesTagExact(TagAvailable)/*QuestStateTag�� Available�� ��*/
            && Q->QuestStartNPCTag == StartNPCTag)/*StartNPCTag�� InteractNPCTag�� ��*/
        {
            Filtered.Add(Q);
        }
    }
    UE_LOG(LogTemp, Log, TEXT(
        "GetQuestsByStateAndStartTag - State: %s, StartTag: %s �� Count: %d"),
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
            && Q->QuestStateTag.MatchesTagExact(TagInProgress)/*InProgress ������ ��*/
            && Q->QuestEndNPCTag == EndNPCTag)/*��ȣ�ۿ� NPC�� EndNPCTag�� ��*/
        {
            Filtered.Add(Q);
        }
    }
    UE_LOG(LogTemp, Log, TEXT(
        "GetQuestsByStateAndEndTag - State: %s, EndTag: %s �� Count: %d"),
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
        if (Q && Q->QuestStateTag.MatchesTagExact(TagInProgress)/*InProgress ������ ��*/)
        {
            if (Q->QuestStartNPCTag == InTag || Q->QuestEndNPCTag == InTag)
            {
                Filtered.Add(Q);
            }
        }
    }
    UE_LOG(LogTemp, Log, TEXT(
        "GetInProgressQuestsWithBothNPC - State: %s, NPCTag: %s �� Count: %d"),
        *TagInProgress.ToString(),
        *InTag.ToString(),
        Filtered.Num()
    );
    return Filtered;
}




//////////////////////////////////////////////////// ��ȣ�ۿ�� �۵� ///////////////////////////////////////////////////

void UQuestManagerComponent::AcceptQuest(UQuestInstance* Q)
{
    if (!GetOwner()->HasAuthority()) return;

    // ����Ʈ ���������� �Ͼ�� �̺�Ʈ(���� ��ȯ ��)
    Q->OnQuestAccepted(); // ���� ��ȯ �Լ�

    // ��ȯ�� ����Ʈ ���¿� ���� �迭 ����
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);
}

void UQuestManagerComponent::ServerAcceptQuest_Implementation(UQuestInstance* Q)
{
    AcceptQuest(Q);
}

void UQuestManagerComponent::CompleteQuest(UQuestInstance* Q)
{
    if (!GetOwner()->HasAuthority()) return;
    // ����Ʈ �Ϸ�� ����� �̺�Ʈ
    UE_LOG(LogTemp, Log, TEXT("QuestManager : CompleteQuest"));

    // ����Ʈ�� ���¸� ��ȯ
    Q->OnQuestCompleted();

    // ��ȯ�� ����Ʈ ���¿� ���� �迭 ����
    SplitQuestsByState(AllQuestArr, AvailableArr, InProgressArr, CompletedArr);

    //// ������ ����Ʈ�� UI�� �˷��� �����ϵ��� ��������Ʈ �߻�
    //OnNPCQuestListUpdated.Broadcast();

    //// ���� �� ��� Ŭ���̾�Ʈ Multicast ȣ��
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


///////////////////////////////////////////////////// ���� ���� �Լ� /////////////////////////////////////////////////////////


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
    OutAvailable.Empty();    // Available �迭 ����
    OutInProgress.Empty();   // InProgress �迭 ����
    OutCompleted.Empty();    // Completed �迭 ����

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
    // ���� ���� PC�� �̿��ϱ�
    if (CachedPlayerController)
    {
        CachedPlayerController->ClientShowQuestListWidget(Available, InProgress, Completed);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestManager : No PC"));
    }
}




/////////////////////////////////////////////////// if Check �Լ� /////////////////////////////////////////////////////

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
    // 1) ���ۿ� ������ �غ�
    TArray<uint8> Buffer;
    FMemoryWriter Writer(Buffer, /* bIsPersistent= */ true);
    Writer.ArIsSaveGame = true;

    // 2) ���� ������ ����
    FQuestSaveData TestData;
    TestData.S_QuestID = FName(TEXT("TestQuest"));
    // ���� S_QuestStatTag�� FGameplayTag Ÿ���̶��
    TestData.S_QuestStateTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"));
    TestData.S_CurrentCount = 5;
    TestData.S_bCanAcceptQuest = true;
    TestData.S_bCanComplete = false;

    // 3) ������� ����ȭ
    Writer << TestData.S_QuestID;
    Writer << TestData.S_QuestStateTag;
    Writer << TestData.S_CurrentCount;
    Writer << TestData.S_bCanAcceptQuest;
    Writer << TestData.S_bCanComplete;

    // 4) ��� ���
    UE_LOG(LogTemp, Log, TEXT("[Quest Debug] Single FQuestSaveData size = %d bytes"), Buffer.Num());
}