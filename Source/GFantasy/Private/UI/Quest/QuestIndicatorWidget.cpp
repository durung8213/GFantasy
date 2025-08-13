// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestIndicatorWidget.h"
#include "UI/Quest/IndicatorBaseWidget.h"
#include "Components/TextBlock.h"
#include "Quest/QuestInstance.h"


void UQuestIndicatorWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // 초기 컬러나 거리 표시 등을 세팅할 수 있습니다.
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


    // 상태·완료 가능 여부에 따라 색 변경
    FLinearColor Color = FLinearColor::White;
    if (QuestInstance->QuestStateTag.MatchesTagExact(
        FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Available"))))
    {
        // 퀘스트 수락가능 -> 노랑
        Color = AvailableColor;
    }
    // InProgress
    else if (QuestInstance->QuestStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"))))
    {
        if (!QuestInstance->bCanComplete)
        {
            // 진행중 -> 빨강
            Color = InProgressColor;
        }
        else
        {
            // 완료 가능 -> 파랑
            Color = IncompleteColor;
        }
    }

    ApplyIndicatorBaseColor(Color);
}

void UQuestIndicatorWidget::UpdateIndicatorData(const FVector2D& ScreenPos, float Distance, bool bOffScreen)
{
    CurrentDistance = Distance;
    bIsOffScreen = bOffScreen;

    // UI 위치 갱신
    // (Add to Viewport했으므로 2D 화면에서 위치가 갱신됨)
    SetPositionInViewport(ScreenPos, false);

    // 거리 텍스트 표시
    if (TargetDistance)
    {
        // 소수점 반올림
        float Rounded = FMath::RoundHalfToEven(Distance);
        TargetDistance->SetText(FText::AsNumber(Rounded));
    }

    // 만약 bOffScreen이면 아이콘을 화살표로 바꾸거나, 회전시키는 등 로직 가능
}
//
//void UQuestIndicatorWidget::ApplyIndicatorColor(const FLinearColor& InColor)
//{
//    if (WBP_IndicatorBase)
//    {
//        // 기본 배경, 마스크, 외곽선, 아이콘 모두 같은 컬러로 바꿔보는 예시
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
        // 기본 배경, 마스크, 외곽선, 아이콘 모두 같은 컬러로 바꿔보는 예시
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


