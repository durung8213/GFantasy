// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPC/NPCQuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HUD/WorldHUD.h"
#include "Quest/QuestInstance.h"
#include "Quest/QuestManagerComponent.h"
#include "GameFramework/PlayerController.h"
#include "UI/NPC/NPCQuestInfoWidget.h"

UNPCQuestInfoWidget* UNPCQuestWidget::SharedInfoWidgetInstance = nullptr;

void UNPCQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuestButton)
	{
		QuestButton->OnClicked.AddDynamic(this, &UNPCQuestWidget::OnQuestButtonClicked);
	}
}

void UNPCQuestWidget::InitializeWithQuest(UQuestInstance* InQuest, UQuestManagerComponent* InManager, UNPCQuestListWidget* InListWidget, FGameplayTag InTag)
{
    Quest = InQuest;    // ���� ����� ����
    Manager = InManager;
    ParentListWidget = InListWidget;
    InteractNPCTag = InTag;

    // 1) ����Ʈ �̸� ����
    if (QuestName && Quest)
    {
        QuestName->SetText(Quest->QuestName);
    }

    // 2) ���� ������ �̸� ����
    if (RewardItemName && Quest)
    {
        // �±׸� �̿��ؼ� ������ �̸� ��������
        RewardItemName->SetText(Quest->RewardItemName);
    }

    // 3) ����Ʈ ���� �±� ���ڿ��� ǥ��
    if (QuestState && Quest)
    {
        QuestState->SetText(Quest->ChangeStateTagToText(Quest->QuestStateTag));

        // InProgress �±� �غ�
        const FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);

        // ���°� InProgress�� ���� ���� ����
        if (Quest->QuestStateTag.MatchesTagExact(InProgressTag))
        {
            if (Quest->bCanComplete)
            {
                // �Ϸ� �����ϸ� �Ķ���
                QuestState->SetColorAndOpacity(FSlateColor(CanCompleteColor));
            }
            else
            {
                // ���� �������� �Ϸ� �Ұ����ϸ� ������
                QuestState->SetColorAndOpacity(FSlateColor(InProgressColor));
            }
        }
        else
        {
            // �� �� ���´� �⺻ �������
            QuestState->SetColorAndOpacity(FSlateColor(NormalColor));
        }
    }
}

void UNPCQuestWidget::CloseNPCQuestWidget()
{
    RemoveFromParent();
}

void UNPCQuestWidget::OnQuestButtonClicked()
{
    // Quest�� ��ȿ���� ������ ����
    if (!Quest)
    {
        return;
    }

    // �÷��̾� ��Ʈ�ѷ� ��������
    if (APlayerController* PC = GetOwningPlayer())
    {
        // ���� ������ ���� �� ���� ����
        if (!SharedInfoWidgetInstance)
        {
            SharedInfoWidgetInstance = CreateWidget<UNPCQuestInfoWidget>(PC, NPCQuestInfoWidgetClass);
            SharedInfoWidgetInstance->AddToViewport();
        }

        // ���Ŀ� �����͸� ����
        SharedInfoWidgetInstance->InitializeWithQuest(Quest, Manager, ParentListWidget, InteractNPCTag);

        // ȭ�鿡 ���ٸ� �ٽ� �߰�
        if (!SharedInfoWidgetInstance->IsInViewport())
        {
            SharedInfoWidgetInstance->AddToViewport();
        }
    }
}
