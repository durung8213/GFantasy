// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestForMainWidget.h"

#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "Quest/QuestInstance.h"
#include "Quest/QuestIndicatorController.h"


void UQuestForMainWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UQuestForMainWidget::InitializeWidget(UQuestInstance* QuestInstance)
{
    if (!QuestInstance)
    {
        return; // ��ȿ���� ���� ��� ����
    }

    // ����Ʈ �ν��Ͻ� ����
    CurrentQuestInstance = QuestInstance;

    // 1) ����Ʈ �̸�
    if (QuestName)
    {
        SetQuestName(QuestInstance);
    }

    // 2) ����Ʈ ����
    if (QuestDescription)
    {
        SetQuestDescription(QuestInstance);
    }

    // 3) ����Ʈ ���� (�±� �̸����� ǥ��)
    if (QuestState)
    {
        SetQuestState(QuestInstance);
    }

    // 4) ���� ���� ����
    if (CurrentCount)
    {
        SetCurrentCount(QuestInstance);
    }

    // 5) �䱸 ����
    if (RequiredCount)
    {
        SetRequiredCount(QuestInstance);
    }

    // 6) �����(%) ��� �� ǥ��
    if (ProgressPercent)
    {
        SetProgressPercent(QuestInstance);
    }
}

#pragma region Set
void UQuestForMainWidget::SetQuestName(UQuestInstance* QuestInstance)
{
    QuestName->SetText(QuestInstance->QuestName);
}

void UQuestForMainWidget::SetQuestDescription(UQuestInstance* QuestInstance)
{
    QuestDescription->SetText(QuestInstance->QuestDescription);
}

void UQuestForMainWidget::SetQuestState(UQuestInstance* QuestInstance)
{
    QuestState->SetText(QuestInstance->ChangeStateTagToText(QuestInstance->QuestStateTag));
}

void UQuestForMainWidget::SetCurrentCount(UQuestInstance* QuestInstance)
{
    CurrentCount->SetText(FText::AsNumber(QuestInstance->CurrentCount));
}

void UQuestForMainWidget::SetRequiredCount(UQuestInstance* QuestInstance)
{
    RequiredCount->SetText(FText::AsNumber(QuestInstance->RequiredCount));
}

void UQuestForMainWidget::SetProgressPercent(UQuestInstance* QuestInstance)
{
    int32 Percent = QuestInstance->QuestProgress;
    UE_LOG(LogTemp, Log, TEXT("QuestWidget : Quest Progress - %d%%"), QuestInstance->QuestProgress);
    FText PercentText = FText::FromString(FString::Printf(TEXT("%d%%"), Percent));

    ProgressPercent->SetText(PercentText);
}

#pragma endregion

FString UQuestForMainWidget::GetStateNameFromTag(const FString& FullTag) const
{
    int32 LastDotIndex = INDEX_NONE;

    if (FullTag.FindLastChar(TEXT('.'), LastDotIndex))
    {
        return FullTag.Mid(LastDotIndex + 1);
    }

    return FullTag;
}