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
        // 체크박스가 바뀔 때 HandleTraceCheckChanged 호출
        TraceOnOffCheck->OnCheckStateChanged.RemoveDynamic(this, &UQuestWidget::HandleTraceCheckChanged);
        TraceOnOffCheck->OnCheckStateChanged.AddDynamic(this, &UQuestWidget::HandleTraceCheckChanged);
    }
}

void UQuestWidget::InitializeWidget(UQuestInstance* QuestInstance)
{
    if (!QuestInstance)
    {
        return; // 유효하지 않은 경우 종료
    }

    // 퀘스트 인스턴스 저장
    CurrentQuestInstance = QuestInstance;

    // 1) 퀘스트 이름
    if (QuestName)
    {
        SetQuestName(QuestInstance);
    }

    // 2) 퀘스트 설명
    if (QuestDescription)
    {
        SetQuestDescription(QuestInstance);
    }

    // 3) 퀘스트 상태 (태그 이름으로 표시)
    if (QuestState)
    {
        SetQuestState(QuestInstance);
    }

    // 4) 현재 진행 수량
    if (CurrentCount)
    {
        SetCurrentCount(QuestInstance);
    }

    // 5) 요구 수량
    if (RequiredCount)
    {
        SetRequiredCount(QuestInstance);
    }

    // 6) 진행률(%) 계산 및 표시
    if (ProgressPercent)
    {
        SetProgressPercent(QuestInstance);
    }

    if (TraceOnOffCheck)
    {
        SetCheckBox(QuestInstance);
    }

    // 위젯 생성 시점에 컨트롤러에 바인딩

    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            if (UQuestIndicatorController* Ctrl = Pawn->FindComponentByClass<UQuestIndicatorController>())
            {
                // 실제 델리게이트 바인딩
                OnTraceCheckChanged.AddUniqueDynamic(Ctrl, &UQuestIndicatorController::HandleTraceToggle);
                
                // 상태 변경 리스너
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
    // 기존 바인딩 해제
    TraceOnOffCheck->OnCheckStateChanged.RemoveAll(this);

    // 저장된 bIsTraced 값으로 체크박스 설정
    TraceOnOffCheck->SetIsChecked(QuestInstance->bIsTraced);

    // 다시 바인딩
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
    // 체크박스를 클릭하면 이 함수가 실행
    UE_LOG(LogTemp, Log, TEXT("QuestIndicator : QuestWidget - HandleTraceCheckChanged"));

    if (CurrentQuestInstance)
    {
        // 델리게이트로 컨트롤러에 알림
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
