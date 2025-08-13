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
    // �̹� �߰��� �ڽ� ������ ��� ����
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
