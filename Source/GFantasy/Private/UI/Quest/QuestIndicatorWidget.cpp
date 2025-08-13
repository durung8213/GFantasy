// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestIndicatorWidget.h"
#include "UI/Quest/IndicatorBaseWidget.h"
#include "Components/TextBlock.h"
#include "Quest/QuestInstance.h"


void UQuestIndicatorWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // �ʱ� �÷��� �Ÿ� ǥ�� ���� ������ �� �ֽ��ϴ�.
}

void UQuestIndicatorWidget::InitQuestIndicatorWidget(AActor* InTargetActor, UQuestInstance* QuestInstance)
{
    TargetActor = InTargetActor;
    CurrentDistance = 0.f;
    bIsOffScreen = false;

    if (TargetName)
    {
        FString Label = QuestInstance->QuestName.ToString();
        TargetName->SetText(FText::FromString(Label));
    }


    // ���¡��Ϸ� ���� ���ο� ���� �� ����
    FLinearColor Color = FLinearColor::White;
    if (QuestInstance->QuestStateTag.MatchesTagExact(
        FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Available"))))
    {
        // ����Ʈ �������� -> ���
        Color = AvailableColor;
    }
    // InProgress
    else if (QuestInstance->QuestStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"))))
    {
        if (!QuestInstance->bCanComplete)
        {
            // ������ -> ����
            Color = InProgressColor;
        }
        else
        {
            // �Ϸ� ���� -> �Ķ�
            Color = IncompleteColor;
        }
    }

    ApplyIndicatorBaseColor(Color);
}

void UQuestIndicatorWidget::UpdateIndicatorData(const FVector2D& ScreenPos, float Distance, bool bOffScreen)
{
    CurrentDistance = Distance;
    bIsOffScreen = bOffScreen;

    // UI ��ġ ����
    // (Add to Viewport�����Ƿ� 2D ȭ�鿡�� ��ġ�� ���ŵ�)
    SetPositionInViewport(ScreenPos, false);

    // �Ÿ� �ؽ�Ʈ ǥ��
    if (TargetDistance)
    {
        // �Ҽ��� �ݿø�
        float Rounded = FMath::RoundHalfToEven(Distance);
        TargetDistance->SetText(FText::AsNumber(Rounded));
    }

    // ���� bOffScreen�̸� �������� ȭ��ǥ�� �ٲٰų�, ȸ����Ű�� �� ���� ����
}
//
//void UQuestIndicatorWidget::ApplyIndicatorColor(const FLinearColor& InColor)
//{
//    if (WBP_IndicatorBase)
//    {
//        // �⺻ ���, ����ũ, �ܰ���, ������ ��� ���� �÷��� �ٲ㺸�� ����
//        WBP_IndicatorBase->SetBaseColor(InColor);
//        WBP_IndicatorBase->SetMaskColor(InColor * 0.5f);
//        WBP_IndicatorBase->SetOutlineColor(InColor * 1.2f);
//    }
//}

void UQuestIndicatorWidget::ApplyIndicatorBaseColor(const FLinearColor& InColor)
{
    UE_LOG(LogTemp, Log, TEXT("QuestIndicator : SetColor"));
    if (WBP_IndicatorBase)
    {
        // �⺻ ���, ����ũ, �ܰ���, ������ ��� ���� �÷��� �ٲ㺸�� ����
        WBP_IndicatorBase->SetBaseColor(InColor);
    }
}

void UQuestIndicatorWidget::UpdateDistance(float InMeters)
{
    if (TargetDistance)
    {
        TargetDistance->SetText(FText::AsNumber(FMath::RoundToInt(InMeters)));
    }
}


