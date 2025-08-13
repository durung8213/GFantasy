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
    // ������ �̸� ������ �ʱ�ȭ
    UFUNCTION(BlueprintCallable, Category = "Indicator")
    void InitQuestIndicatorWidget(AActor* InTargetActor, UQuestInstance* QuestInstance);

    // �� ������ Manager�� ȣ��, �Ÿ�/��ũ����ǥ�� ����
    UFUNCTION(BlueprintCallable, Category = "Indicator")
    void UpdateIndicatorData(const FVector2D& ScreenPos, float Distance, bool bOffScreen);


    //// �÷��� �� ���� �ٲ��ִ� ����
    //UFUNCTION(BlueprintCallable, Category = "QuestIndicator")
    //void ApplyIndicatorColor(const FLinearColor& InColor);

    // ���̽� �÷��� �ٲ��ִ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "QuestIndicator")
    void ApplyIndicatorBaseColor(const FLinearColor& InColor);

    // �Ÿ��� �ؽ�Ʈ ������Ʈ �� ȣ��
    UFUNCTION(BlueprintCallable, Category = "QuestIndicator")
    void UpdateDistance(float InMeters);


protected:
    virtual void NativeConstruct() override;
	
	

public:
    // �� ������ � QuestInstance ������ ����
    UPROPERTY()
    UQuestInstance* OwningQuestInstance = nullptr;


    // ǥ���� ���� (�ʿ�� �ٸ� ���� ��ȸ ����)
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    AActor* TargetActor;

    // ǥ���� �Ÿ�
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    float CurrentDistance;

    // ������ũ�� ����
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    bool bIsOffScreen;

    // Target���� �ٸ� ��
    UPROPERTY(BlueprintReadOnly, Category = "Indicator")
    FLinearColor IndicatorColor;



    // �����Ϳ��� ������ �⺻ �����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Indicator|Colors")
    FLinearColor AvailableColor = FLinearColor::Yellow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Indicator|Colors")
    FLinearColor InProgressColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Indicator|Colors")
    FLinearColor IncompleteColor = FLinearColor::Blue;


public:
    // CanvasPanel �� VerticalBox �� SizeBox �� WBP_IndicatorBase
    UPROPERTY(meta = (BindWidget))
    class UIndicatorBaseWidget* WBP_IndicatorBase;

protected:
    // Ÿ�� �̸� �ؽ�Ʈ
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TargetName;

    // Ÿ�� �Ÿ� �ؽ�Ʈ ("100")
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TargetDistance;

	
};
