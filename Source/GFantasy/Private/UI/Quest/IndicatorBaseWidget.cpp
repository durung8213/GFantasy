// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/IndicatorBaseWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"



void UIndicatorBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // (�ʿ�� �ʱ� �÷��� �������� ������ �� �ֽ��ϴ�)
}

void UIndicatorBaseWidget::SetBaseColor(const FLinearColor& InColor)
{
    UE_LOG(LogTemp, Warning, TEXT("QuestIndicatorBaseWidget : SetBaseColor called; img_Icon_Base is %s"),
        img_Icon_Base ? TEXT("valid") : TEXT("null"));  // �� ���⿡ �ߴ���/�α�
    if (img_Icon_Base)
    {
        // �̹����� Tint �÷��� ����
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
//    // ��: GlassMask �� �����ϴ� Image ������Ʈ�� img_Icon_Mask1/2 �� �ϳ����
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
//        // RenderTransform ���� ȸ��
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
//        // �߾� ������ ������ ����
//        FWidgetTransform t = img_Icon->RenderTransform;
//        t.Scale = FVector2D(InScale, InScale);
//        img_Icon->SetRenderTransform(t);
//    }
//}
//
