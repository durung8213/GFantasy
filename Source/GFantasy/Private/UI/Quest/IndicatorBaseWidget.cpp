// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/IndicatorBaseWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"



void UIndicatorBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // (필요시 초기 컬러나 스케일을 설정할 수 있습니다)
}

void UIndicatorBaseWidget::SetBaseColor(const FLinearColor& InColor)
{
    UE_LOG(LogTemp, Warning, TEXT("QuestIndicatorBaseWidget : SetBaseColor called; img_Icon_Base is %s"),
        img_Icon_Base ? TEXT("valid") : TEXT("null"));  // ← 여기에 중단점/로그
    if (img_Icon_Base)
    {
        // 이미지의 Tint 컬러를 변경
        img_Icon_Base->SetColorAndOpacity(InColor);
    }
}
//
//void UIndicatorBaseWidget::SetMaskColor(const FLinearColor& InColor)
//{
//    if (img_Icon_Mask1)
//    {
//        img_Icon_Mask1->SetColorAndOpacity(InColor);
//    }
//    if (img_Icon_Mask2)
//    {
//        img_Icon_Mask2->SetColorAndOpacity(InColor);
//    }
//}
//
//void UIndicatorBaseWidget::SetOutlineColor(const FLinearColor& InColor)
//{
//    if (img_Icon_Outline1)
//    {
//        img_Icon_Outline1->SetColorAndOpacity(InColor);
//    }
//    if (img_Icon_Outline2)
//    {
//        img_Icon_Outline2->SetColorAndOpacity(InColor);
//    }
//}
//
//void UIndicatorBaseWidget::SetGlassMaskColor(const FLinearColor& InColor)
//{
//    // 예: GlassMask 에 대응하는 Image 콤포넌트가 img_Icon_Mask1/2 중 하나라면
//    if (img_Icon_Mask1)
//    {
//        img_Icon_Mask1->SetColorAndOpacity(InColor);
//    }
//}
//
//void UIndicatorBaseWidget::SetOverallAngle(float InDegrees)
//{
//    if (img_Icon_Base)
//    {
//        // RenderTransform 으로 회전
//        FWidgetTransform t = img_Icon_Base->RenderTransform;
//        t.Angle = InDegrees;
//        img_Icon_Base->SetRenderTransform(t);
//    }
//}
//
//void UIndicatorBaseWidget::SetIconScale(float InScale)
//{
//    if (img_Icon)
//    {
//        // 중앙 아이콘 스케일 설정
//        FWidgetTransform t = img_Icon->RenderTransform;
//        t.Scale = FVector2D(InScale, InScale);
//        img_Icon->SetRenderTransform(t);
//    }
//}
//
