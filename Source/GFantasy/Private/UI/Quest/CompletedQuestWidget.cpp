// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/CompletedQuestWidget.h"
#include "Components/ScrollBox.h"
#include "UI/Quest/QuestWidget.h"
#include "Quest/QuestInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UCompletedQuestWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!QuestScrollBox)
    {
        UE_LOG(LogTemp, Log, TEXT("CompletedQuestWidget : No Bind QuestScrollBox"));
        return;
    }
    // �̹� �߰��� �ڽ� ������ ��� ����
    QuestScrollBox->ClearChildren();
}

void UCompletedQuestWidget::PopulateCompletedQuests(const TArray<class UQuestInstance*>& QuestInstances)
{
    UE_LOG(LogTemp, Log, TEXT("CompletedQuestWidget : In PopulateAvailableQuests"));
    UE_LOG(LogTemp, Warning, TEXT("PopulateCompletedQuests on %s (%p), ScrollBox=%p, Children=%d"),
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
            UE_LOG(LogTemp, Warning, TEXT("CompletedQuestWidget: No OwningPlayer(PC)"));
            return;
        }
    }
}

void UCompletedQuestWidget::CheckQuestInstance(UQuestInstance* QI)
{
    if (!QI)
    {
        UE_LOG(LogTemp, Log, TEXT("CompletedQuestWidget : No QuestInctance"));
        return;
    }
    if (!QuestWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("CompletedQuestWidget : No QuestWidgetClass"));
        return;
    }
}