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
    // --- BindWidget 으로 블루프린트 쪽 위젯 가져오기 ---
    // (블루프린트 UMG 에서 이름이 똑같아야 합니다)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Base;       ///< 기본 아이콘 배경

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Mask1;      ///< 마스크 1

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Outline1;   ///< 외곽선 1

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Outline2;   ///< 외곽선 2

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon;            ///< 중앙 아이콘

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* Text_IconText;   ///< 중앙 텍스트 ('E' 등)

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UImage* img_Icon_Mask2;      ///< 마스크 2

public:
    // 기본 컬러(Background) 설정
    // 이것만 사용할 것임
    UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    void SetBaseColor(const FLinearColor& InColor);

    //// 마스크 컬러 설정
    //UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    //void SetMaskColor(const FLinearColor& InColor);

    //// 외곽선 컬러 설정
    //UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    //void SetOutlineColor(const FLinearColor& InColor);

    //// 글래스(Glass) 마스크 컬러 설정
    //UFUNCTION(BlueprintCallable, Category = "Indicator|Color")
    //void SetGlassMaskColor(const FLinearColor& InColor);

    //// 전체 아이콘 각도(회전) 설정
    //UFUNCTION(BlueprintCallable, Category = "Indicator")
    //void SetOverallAngle(float InDegrees);

    //// 아이콘 스케일 설정
    //UFUNCTION(BlueprintCallable, Category = "Indicator")
    //void SetIconScale(float InScale);

protected:
    // NativeConstruct에서 초기화가 필요하면 추가
    virtual void NativeConstruct() override;
	
	
};
