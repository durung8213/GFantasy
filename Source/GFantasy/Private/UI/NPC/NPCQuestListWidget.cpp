// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPC/NPCQuestListWidget.h"
#include "Components/ScrollBox.h"
#include "Quest/QuestManagerComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "HUD/WorldHUD.h"
#include "Quest/QuestInstance.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "UI/NPC/NPCQuestWidget.h"
#include "UI/NPC/NPCQuestInfoWidget.h"

void UNPCQuestListWidget::NativeConstruct()
{
	Super::NativeConstruct();

    if (ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &UNPCQuestListWidget::OnExitButtonClicked);
    }

    // 플레이어 컨트롤러 → 플레이어 스테이트 → QuestManagerComponent 캐싱
    if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        if (AGASWorldPlayerState* PS = PC->GetPlayerState<AGASWorldPlayerState>())
        {
            QuestManager = PS->FindComponentByClass<UQuestManagerComponent>();
        }
    }

    if (QuestManager)
    {
        // QuestInstance의 상태가 바뀔 때마다 전체 리스트를 새로 그리도록 바인딩
        for (UQuestInstance* Q : QuestManager->AllQuestArr)
        {
            if (Q)
            {
                Q->OnNPCQuestListUpdated.AddUniqueDynamic(this, &UNPCQuestListWidget::RefreshQuestList);
            }
        }
    }

}

void UNPCQuestListWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (QuestManager)
    {
        // 메모리 누수 방지를 위해 언바인딩
        for (UQuestInstance* Q : QuestManager->AllQuestArr)
        {
            if (Q)
            {
                Q->OnNPCQuestListUpdated.RemoveDynamic(this, &UNPCQuestListWidget::RefreshQuestList);
            }
        }
    }
}

void UNPCQuestListWidget::RefreshQuestList()
{
    // 델리게이트로 호출될 때는 기존 리스트를 지우고 다시 초기화
    InitializeQuestList();
}

void UNPCQuestListWidget::InitializeWithNPCTag(FGameplayTag InNPCTag)
{
    UE_LOG(LogTemp, Log, TEXT("NPCQuestListWidget : NPCNameTag = %s"), *InNPCTag.ToString());

	InteractNPCTag = InNPCTag;
    UE_LOG(LogTemp, Log, TEXT("NPCQuestListWidget : NPCNameTag = %s"), *InteractNPCTag.ToString());

    // 초기화한 NPCTag로 퀘스트 목록도 초기화해주기
    InitializeQuestList();
}

void UNPCQuestListWidget::OnExitButtonClicked()
{
    // 창 닫기 버튼

    // 먼저 OwningPlayer 를 캐시
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("NPCQuestListWidget: OwningPlayer가 없습니다."));
        return;
    }

    // Info 위젯 팝업들 모두 제거
    for (UNPCQuestWidget* QuestWidget : CachedQuestWidgets)
    {
        if (QuestWidget/* && QuestWidget->NPCQuestInfoWidgetInstance*/)
        {
            QuestWidget/*->NPCQuestInfoWidgetInstance*/->RemoveFromParent();
        }
    }

    CachedQuestWidgets.Empty();
    NPCQuestScrollBox->ClearChildren();

    if (UNPCQuestWidget::SharedInfoWidgetInstance)
    {
        UNPCQuestWidget::SharedInfoWidgetInstance->RemoveFromParent();
        UNPCQuestWidget::SharedInfoWidgetInstance = nullptr;
    }


    // 마우스 커서 감추고, 게임 전용 모드로 복귀
    PC->bShowMouseCursor = false;
    FInputModeGameOnly GameInputMode;
    PC->SetInputMode(GameInputMode);

    // 이제 안전하게 위젯 제거
    RemoveFromParent();

    // HUD 쪽 핸들링도 있다면
    if (AWorldHUD* HUD = Cast<AWorldHUD>(PC->GetHUD()))
    {
        HUD->NPCInteractionWidget = nullptr;
    }
}

void UNPCQuestListWidget::InitializeQuestList()
{
    if (!NPCQuestScrollBox) return;

    for (UNPCQuestWidget* Widget : CachedQuestWidgets)
    {
        if (Widget)
        {
            Widget->RemoveFromParent();
        }
    }
    CachedQuestWidgets.Empty();
    NPCQuestScrollBox->ClearChildren();
    
    // QuestManagerComponent 찾기

    if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        if (AGASWorldPlayerState* PS = PC->GetPlayerState<AGASWorldPlayerState>())
        {
            // PS에 붙어있는 QuestManagerComponent 찾아오기
            QuestManager = PS->FindComponentByClass<UQuestManagerComponent>();
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("NPCQuestListWidget : No PS"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("NPCQuestListWidget : No PC"));
    }
    if (!QuestManager) return;

    for (UQuestInstance* q : QuestManager->AllQuestArr)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestTag : %s"), *q->QuestStateTag.ToString());
    }

    // Tag값과 맞는 QuestManager의 퀘스트 가져오기
    // Available 상태일때 && 상호작용 NPC가 StartNPC일 때
    TArray<UQuestInstance*> AvailableQuests = QuestManager->GetAvailableQuestsWithStartNPC(InteractNPCTag);
    // InProgressQuests 상태 && NPC가 Start || End일 때
    TArray<UQuestInstance*> InProgressQuests = QuestManager->GetInProgressQuestsWithBothNPC(InteractNPCTag);

    UE_LOG(LogTemp, Log, TEXT("QuestListWidget : %s has %d Available quests"), *InteractNPCTag.ToString(), AvailableQuests.Num());
    UE_LOG(LogTemp, Log, TEXT("QuestListWidget : %s has %d InProgress quests"), *InteractNPCTag.ToString(), InProgressQuests.Num());

    // Available 루프
    for (UQuestInstance* Q : AvailableQuests)
    {
        if (!Q) continue;
        UE_LOG(LogTemp, Log, TEXT("[Available] %s (%d/%d)"), *Q->QuestName.ToString(), Q->CurrentCount, Q->RequiredCount);

        UNPCQuestWidget* Entry = CreateWidget<UNPCQuestWidget>(GetOwningPlayer(), NPCQuestWidgetClass);
        if (!Entry) continue;
        Entry->InitializeWithQuest(Q, QuestManager, this, InteractNPCTag);
        NPCQuestScrollBox->AddChild(Entry);
        CachedQuestWidgets.Add(Entry);
    }

    // InProgress 루프
    for (UQuestInstance* Q : InProgressQuests)
    {
        if (!Q) continue;
        UE_LOG(LogTemp, Log, TEXT("[InProgress] %s (%d/%d)"), *Q->QuestName.ToString(), Q->CurrentCount, Q->RequiredCount);

        UNPCQuestWidget* Entry = CreateWidget<UNPCQuestWidget>(GetOwningPlayer(), NPCQuestWidgetClass);
        if (!Entry) continue;
        Entry->InitializeWithQuest(Q, QuestManager, this, InteractNPCTag);
        NPCQuestScrollBox->AddChild(Entry);
        CachedQuestWidgets.Add(Entry);
    }
}