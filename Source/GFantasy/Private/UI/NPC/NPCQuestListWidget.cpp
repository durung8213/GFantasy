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

    // �÷��̾� ��Ʈ�ѷ� �� �÷��̾� ������Ʈ �� QuestManagerComponent ĳ��
    if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        if (AGASWorldPlayerState* PS = PC->GetPlayerState<AGASWorldPlayerState>())
        {
            QuestManager = PS->FindComponentByClass<UQuestManagerComponent>();
        }
    }

    if (QuestManager)
    {
        // QuestInstance�� ���°� �ٲ� ������ ��ü ����Ʈ�� ���� �׸����� ���ε�
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
        // �޸� ���� ������ ���� ����ε�
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
    // ��������Ʈ�� ȣ��� ���� ���� ����Ʈ�� ����� �ٽ� �ʱ�ȭ
    InitializeQuestList();
}

void UNPCQuestListWidget::InitializeWithNPCTag(FGameplayTag InNPCTag)
{
    UE_LOG(LogTemp, Log, TEXT("NPCQuestListWidget : NPCNameTag = %s"), *InNPCTag.ToString());

	InteractNPCTag = InNPCTag;
    UE_LOG(LogTemp, Log, TEXT("NPCQuestListWidget : NPCNameTag = %s"), *InteractNPCTag.ToString());

    // �ʱ�ȭ�� NPCTag�� ����Ʈ ��ϵ� �ʱ�ȭ���ֱ�
    InitializeQuestList();
}

void UNPCQuestListWidget::OnExitButtonClicked()
{
    // â �ݱ� ��ư

    // ���� OwningPlayer �� ĳ��
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("NPCQuestListWidget: OwningPlayer�� �����ϴ�."));
        return;
    }

    // Info ���� �˾��� ��� ����
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


    // ���콺 Ŀ�� ���߰�, ���� ���� ���� ����
    PC->bShowMouseCursor = false;
    FInputModeGameOnly GameInputMode;
    PC->SetInputMode(GameInputMode);

    // ���� �����ϰ� ���� ����
    RemoveFromParent();

    // HUD �� �ڵ鸵�� �ִٸ�
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
    
    // QuestManagerComponent ã��

    if (APlayerController* PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        if (AGASWorldPlayerState* PS = PC->GetPlayerState<AGASWorldPlayerState>())
        {
            // PS�� �پ��ִ� QuestManagerComponent ã�ƿ���
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

    // Tag���� �´� QuestManager�� ����Ʈ ��������
    // Available �����϶� && ��ȣ�ۿ� NPC�� StartNPC�� ��
    TArray<UQuestInstance*> AvailableQuests = QuestManager->GetAvailableQuestsWithStartNPC(InteractNPCTag);
    // InProgressQuests ���� && NPC�� Start || End�� ��
    TArray<UQuestInstance*> InProgressQuests = QuestManager->GetInProgressQuestsWithBothNPC(InteractNPCTag);

    UE_LOG(LogTemp, Log, TEXT("QuestListWidget : %s has %d Available quests"), *InteractNPCTag.ToString(), AvailableQuests.Num());
    UE_LOG(LogTemp, Log, TEXT("QuestListWidget : %s has %d InProgress quests"), *InteractNPCTag.ToString(), InProgressQuests.Num());

    // Available ����
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

    // InProgress ����
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