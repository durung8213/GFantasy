// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestWidget.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "Quest/QuestInstance.h"
#include "Quest/QuestIndicatorController.h"


void UQuestWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TraceOnOffCheck)
    {
        // üũ�ڽ��� �ٲ� �� HandleTraceCheckChanged ȣ��
        TraceOnOffCheck->OnCheckStateChanged.RemoveDynamic(this, &UQuestWidget::HandleTraceCheckChanged);
        TraceOnOffCheck->OnCheckStateChanged.AddDynamic(this, &UQuestWidget::HandleTraceCheckChanged);
    }
}

void UQuestWidget::InitializeWidget(UQuestInstance* QuestInstance)
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

    if (TraceOnOffCheck)
    {
        SetCheckBox(QuestInstance);
    }

    // ���� ���� ������ ��Ʈ�ѷ��� ���ε�

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            if (UQuestIndicatorController* Ctrl = Pawn->FindComponentByClass<UQuestIndicatorController>())
            {
                // ���� ��������Ʈ ���ε�
                OnTraceCheckChanged.AddUniqueDynamic(Ctrl, &UQuestIndicatorController::HandleTraceToggle);
                
                // ���� ���� ������
                QuestInstance->OnQuestStateChanged.AddUniqueDynamic(Ctrl, &UQuestIndicatorController::OnQuestStateChanged);
                
                UE_LOG(LogTemp, Log, TEXT("QuestWidget : Bound to IndicatorController"));
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("QuestWidget : No QuestIndicatorController on Pawn"));
            }
        }
    }
}

#pragma region Set
void UQuestWidget::SetQuestName(UQuestInstance* QuestInstance)
{
    QuestName->SetText(QuestInstance->QuestName);
}

void UQuestWidget::SetQuestDescription(UQuestInstance* QuestInstance)
{
    QuestDescription->SetText(QuestInstance->QuestDescription);
}

void UQuestWidget::SetQuestState(UQuestInstance* QuestInstance)
{
    QuestState->SetText(QuestInstance->ChangeStateTagToText(QuestInstance->QuestStateTag));
}

void UQuestWidget::SetCurrentCount(UQuestInstance* QuestInstance)
{
    CurrentCount->SetText(FText::AsNumber(QuestInstance->CurrentCount));
}

void UQuestWidget::SetRequiredCount(UQuestInstance* QuestInstance)
{
    RequiredCount->SetText(FText::AsNumber(QuestInstance->RequiredCount));
}

void UQuestWidget::SetProgressPercent(UQuestInstance* QuestInstance)
{
    int32 Percent = QuestInstance->QuestProgress;
    UE_LOG(LogTemp, Log, TEXT("QuestWidget : Quest Progress - %d%%"), QuestInstance->QuestProgress);
    FText PercentText = FText::FromString(FString::Printf(TEXT("%d%%"), Percent));
    
    ProgressPercent->SetText(PercentText);
}

void UQuestWidget::SetCheckBox(UQuestInstance* QuestInstance)
{
    // ���� ���ε� ����
    TraceOnOffCheck->OnCheckStateChanged.RemoveAll(this);

    // ����� bIsTraced ������ üũ�ڽ� ����
    TraceOnOffCheck->SetIsChecked(QuestInstance->bIsTraced);

    // �ٽ� ���ε�
    TraceOnOffCheck->OnCheckStateChanged.AddDynamic(this, &UQuestWidget::HandleTraceCheckChanged);
}

void UQuestWidget::BindToQuest(UQuestInstance* QuestInstance)
{
    if (QuestInstance)
    {
        SetCurrentCount(QuestInstance);
        QuestInstance->OnCountChanged.AddDynamic(this, &UQuestWidget::SetCurrentCount);
    }
}
#pragma endregion

void UQuestWidget::HandleTraceCheckChanged(bool bIsChecked)
{
    // üũ�ڽ��� Ŭ���ϸ� �� �Լ��� ����
    UE_LOG(LogTemp, Log, TEXT("QuestIndicator : QuestWidget - HandleTraceCheckChanged"));

    if (CurrentQuestInstance)
    {
        // ��������Ʈ�� ��Ʈ�ѷ��� �˸�
        OnTraceCheckChanged.Broadcast(CurrentQuestInstance, bIsChecked);
        UE_LOG(LogTemp, Log, TEXT("QuestIndicator : QuestWidget - Broadcast"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("QuestIndicator : QuestWidget - No QuestInstance"));
    }
}

FString UQuestWidget::GetStateNameFromTag(const FString& FullTag) const
{
    int32 LastDotIndex = INDEX_NONE;

    if (FullTag.FindLastChar(TEXT('.'), LastDotIndex))
    {
        return FullTag.Mid(LastDotIndex + 1);
    }

    return FullTag;
}
