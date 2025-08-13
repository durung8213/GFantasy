// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseUserQuestWidget.h"
#include "IndicatorBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UIndicatorBaseWidget : public UBaseUserQuestWidget
{
	GENERATED_BODY()
	

public:
    // --- BindWidget ���� �������Ʈ �� ���� �������� ---
    // (�������Ʈ UMG ���� �̸��� �Ȱ��ƾ� �մϴ�)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Base;       ///< �⺻ ������ ���

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Mask1;      ///< ����ũ 1

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Outline1;   ///< �ܰ��� 1

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Outline2;   ///< �ܰ��� 2

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon;            ///< �߾� ������

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* Text_IconText;   ///< �߾� �ؽ�Ʈ ('E' ��)

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Mask2;      ///< ����ũ 2

public:
    // �⺻ �÷�(Background) ����
    // �̰͸� ����� ����
    UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    void SetBaseColor(const FLinearColor& InColor);

    //// ����ũ �÷� ����
    //UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    //void SetMaskColor(const FLinearColor& InColor);

    //// �ܰ��� �÷� ����
    //UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    //void SetOutlineColor(const FLinearColor& InColor);

    //// �۷���(Glass) ����ũ �÷� ����
    //UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    //void SetGlassMaskColor(const FLinearColor& InColor);

    //// ��ü ������ ����(ȸ��) ����
    //UFUNCTION(BlueprintCallable, Category = "Indicator")
    //void SetOverallAngle(float InDegrees);

    //// ������ ������ ����
    //UFUNCTION(BlueprintCallable, Category = "Indicator")
    //void SetIconScale(float InScale);

protected:
    // NativeConstruct���� �ʱ�ȭ�� �ʿ��ϸ� �߰�
    virtual void NativeConstruct() override;
	
	
};
