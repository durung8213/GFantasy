// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/InProgressQuestWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestInstance.h"
#include "UI/Quest/QuestWidget.h"
#include "Components/ScrollBox.h"

void UInProgressQuestWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!QuestScrollBox)
    {
        UE_LOG(LogTemp, Log, TEXT("InProgressQuestWidget : No Bind QuestScrollBox"));
        return;
    }
    // 이미 추가된 자식 위젯은 모두 제거
    QuestScrollBox->ClearChildren();
}

void UInProgressQuestWidget::PopulateInProgressQuests(const TArray<UQuestInstance*>& QuestInstances)
{
    UE_LOG(LogTemp, Log, TEXT("InProgressQuestWidget : In PopulateAvailableQuests"));
    UE_LOG(LogTemp, Warning, TEXT("PopulateInProgressQuests on %s (%p), ScrollBox=%p, Children=%d"),
        *GetName(),
        this,
        QuestScrollBox,
        QuestScrollBox ? QuestScrollBox->GetChildrenCount() : -1
    );
    QuestScrollBox->ClearChildren();

    // QuestInstances 수 만큼 QuestWidget을 생성하여 추가
    for (UQuestInstance* QuestInstance : QuestInstances)
    {
        CheckQuestInstance(QuestInstance);

        // OwningPlayer 컨텍스트로 위젯 생성
        if (APlayerController* PC = GetOwningPlayer())
        {
            UQuestWidget* QuestW = CreateWidget<UQuestWidget>(PC, QuestWidgetClass);
            if (QuestW)
            {
                // 퀘스트 데이터를 위젯에 초기화
                QuestW->InitializeWidget(QuestInstance);
                // 스크롤 박스에 추가
                QuestScrollBox->AddChild(QuestW);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InProgressQuestWidget: No OwningPlayer(PC)"));
            return;
        }
    }
}

void UInProgressQuestWidget::CheckQuestInstance(UQuestInstance* QI)
{
    if (!QI)
    {
        UE_LOG(LogTemp, Log, TEXT("InProgressQuestWidget : No QuestInctance"));
        return;
    }
    if (!QuestWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("InProgressQuestWidget : No QuestWidgetClass"));
        return;
    }
}