// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/AvailableQuestWidget.h"
#include "Components/ScrollBox.h"
#include "UI/Quest/QuestWidget.h"
#include "Quest/QuestInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


void UAvailableQuestWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (!QuestScrollBox)
    {
        UE_LOG(LogTemp, Log, TEXT("AvailableQuestWidget : No Bind QuestScrollBox"));
        return;
    }
    // 이미 추가된 자식 위젯은 모두 제거
    QuestScrollBox->ClearChildren();
}

void UAvailableQuestWidget::PopulateAvailableQuests(const TArray<UQuestInstance*>& QuestInstances)
{
    UE_LOG(LogTemp, Log, TEXT("AvailableQuestWidget : In PopulateAvailableQuests"));
    UE_LOG(LogTemp, Warning, TEXT("PopulateAvailableQuests on %s (%p), ScrollBox=%p, Children=%d"),
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
            UE_LOG(LogTemp, Warning, TEXT("AvailableQuestWidget: No OwningPlayer(PC)"));
            return;
        }
    }
}

void UAvailableQuestWidget::CheckQuestInstance(UQuestInstance* QI)
{
    if (!QI)
    {
        UE_LOG(LogTemp, Log, TEXT("AvailableQuestWidget : No QuestInctance"));
        return;
    }
    if (!QuestWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("AvailableQuestWidget : No QuestWidgetClass"));
        return;
    }
}
