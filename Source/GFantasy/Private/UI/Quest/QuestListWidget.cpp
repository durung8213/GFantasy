// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestListWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Quest/AvailableQuestWidget.h"
#include "UI/Quest/AvailableQuestWidget.h"
#include "UI/Quest/InProgressQuestWidget.h"
#include "UI/Quest/CompletedQuestWidget.h"
#include "GameplayTagContainer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void UQuestListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // ���⼭ �ʱ� �� ���� �� �߰� ������ �ʿ��ϸ� �ۼ�

    if (AvailableButton)
    {
        AvailableButton->OnClicked.AddDynamic(this, &UQuestListWidget::OnAvailableButtonClicked);
    }
    if (InProgressButton)
    {
        InProgressButton->OnClicked.AddDynamic(this, &UQuestListWidget::OnInProgressButtonClicked);
    }
    if (CompletedButton)
    {
        CompletedButton->OnClicked.AddDynamic(this, &UQuestListWidget::OnCompletedButtonClicked);
    }
}

void UQuestListWidget::ShowAllQuest(const TArray<class UQuestInstance*>& Available, const TArray<class UQuestInstance*>& InProgress, const TArray<class UQuestInstance*>& Completed)
{
    // ���ο��� �� �� �Լ� ȣ��
    if (WBP_AvailableQuest)
    {
        WBP_AvailableQuest->PopulateAvailableQuests(Available);
    }
    if (WBP_InProgressQuest)
    {
        WBP_InProgressQuest->PopulateInProgressQuests(InProgress);
    }
    if (WBP_CompletedQuest)
    {
        WBP_CompletedQuest->PopulateCompletedQuests(Completed);
    }
}

void UQuestListWidget::ShowAvailableTab(const TArray<UQuestInstance*>& QuestInstances)
{
    if (!QuestSwitcher)
    {
        UE_LOG(LogTemp, Log, TEXT("[QuestListWidget] No QuestSwitcher"));
        return;
    }

    if (!WBP_AvailableQuest)
    {
        UE_LOG(LogTemp, Log, TEXT("[QuestListWidget] No AvailableQuestWidget"));
        return;
    }
    // AvailableQuestWidget�� ����Ʈ ����
    WBP_AvailableQuest->PopulateAvailableQuests(QuestInstances);
}

void UQuestListWidget::ShowInProgressTab(const TArray<class UQuestInstance*>& QuestInstances)
{
    if (!QuestSwitcher)
    {
        UE_LOG(LogTemp, Log, TEXT("[QuestListWidget] No QuestSwitcher"));
        return;
    }

    if (!WBP_InProgressQuest)
    {
        UE_LOG(LogTemp, Log, TEXT("[QuestListWidget] No AvailableQuestWidget"));
        return;
    }

    // AvailableQuestWidget�� ����Ʈ ����
    WBP_InProgressQuest->PopulateInProgressQuests(QuestInstances);
}

void UQuestListWidget::ShowCompletedTab(const TArray<class UQuestInstance*>& QuestInstances)
{
    if (!QuestSwitcher)
    {
        UE_LOG(LogTemp, Log, TEXT("[QuestListWidget] No QuestSwitcher"));
        return;
    }

    if (!WBP_CompletedQuest)
    {
        UE_LOG(LogTemp, Log, TEXT("[QuestListWidget] No AvailableQuestWidget"));
        return;
    }

    // AvailableQuestWidget�� ����Ʈ ����
    WBP_CompletedQuest->PopulateCompletedQuests(QuestInstances);
}

void UQuestListWidget::OnAvailableButtonClicked()
{
    QuestSwitcher->SetActiveWidgetIndex(0);
}

void UQuestListWidget::OnInProgressButtonClicked()
{
    QuestSwitcher->SetActiveWidgetIndex(1);
}

void UQuestListWidget::OnCompletedButtonClicked()
{
    QuestSwitcher->SetActiveWidgetIndex(2);
}

