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
    // �̹� �߰��� �ڽ� ������ ��� ����
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

    // QuestInstances �� ��ŭ QuestWidget�� �����Ͽ� �߰�
    for (UQuestInstance* QuestInstance : QuestInstances)
    {
        CheckQuestInstance(QuestInstance);

        // OwningPlayer ���ؽ�Ʈ�� ���� ����
        if (APlayerController* PC = GetOwningPlayer())
        {
            UQuestWidget* QuestW = CreateWidget<UQuestWidget>(PC, QuestWidgetClass);
            if (QuestW)
            {
                // ����Ʈ �����͸� ������ �ʱ�ȭ
                QuestW->InitializeWidget(QuestInstance);
                // ��ũ�� �ڽ��� �߰�
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