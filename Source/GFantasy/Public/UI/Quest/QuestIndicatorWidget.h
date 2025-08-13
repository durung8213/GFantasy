// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseUserQuestWidget.h"
#include "QuestIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UQuestIndicatorWidget : public UBaseUserQuestWidget
{
	GENERATED_BODY()

public:
    // 액터의 이름 정보를 초기화
    UFUNCTION(BlueprintCallable, Category = "Indicator")
    void InitQuestIndicatorWidget(AActor* InTargetActor, UQuestInstance* QuestInstance);

    // 매 프레임 Manager가 호출, 거리/스크린좌표를 전달
    UFUNCTION(BlueprintCallable, Category = "Indicator")
    void UpdateIndicatorData(const FVector2D& ScreenPos, float Distance, bool bOffScreen);


    //// 컬러를 한 번에 바꿔주는 헬퍼
    //UFUNCTION(BlueprintCallable, Category = "QuestIndicator")
    //void ApplyIndicatorColor(const FLinearColor& InColor);

    // 베이스 컬러를 바꿔주는 함수
    UFUNCTION(BlueprintCallable, Category = "QuestIndicator")
    void ApplyIndicatorBaseColor(const FLinearColor& InColor);

    // 거리나 텍스트 업데이트 시 호출
    UFUNCTION(BlueprintCallable, Category = "QuestIndicator")
    void UpdateDistance(float InMeters);


protected:
    virtual void NativeConstruct() override;
	
	

public:
    // 이 위젯이 어떤 QuestInstance 용인지 저장
    UPROPERTY()
    UQuestInstance* OwningQuestInstance = nullptr;


    // 표시할 액터 (필요시 다른 정보 조회 가능)
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    AActor* TargetActor;

    // 표시할 거리
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    float CurrentDistance;

    // 오프스크린 여부
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    bool bIsOffScreen;

    // Target마다 다른 색
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    FLinearColor IndicatorColor;



    // 에디터에서 지정할 기본 색상들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Indicator|Colors")
    FLinearColor AvailableColor = FLinearColor::Yellow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Indicator|Colors")
    FLinearColor InProgressColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Indicator|Colors")
    FLinearColor IncompleteColor = FLinearColor::Blue;


public:
    // CanvasPanel → VerticalBox → SizeBox → WBP_IndicatorBase
    UPROPERTY(meta = (BindWidget))
    class UIndicatorBaseWidget* WBP_IndicatorBase;

protected:
    // 타겟 이름 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TargetName;

    // 타겟 거리 텍스트 ("100")
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TargetDistance;

	
};
