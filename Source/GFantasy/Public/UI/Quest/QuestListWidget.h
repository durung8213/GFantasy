// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseUserQuestWidget.h"
#include "QuestListWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UQuestListWidget : public UBaseUserQuestWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    void ShowAllQuest(
        const TArray<class UQuestInstance*>& Available,
        const TArray<class UQuestInstance*>& InProgress,
        const TArray<class UQuestInstance*>& Completed
    );

    //시작 가능 퀘스트 탭을 열고, 내부 AvailableQuestWidget에 데이터 전달
    UFUNCTION(BlueprintCallable)
    void ShowAvailableTab(const TArray<class UQuestInstance*>&QuestInstances);

    UFUNCTION(BlueprintCallable)
    void ShowInProgressTab(const TArray<class UQuestInstance*>&QuestInstances);

    UFUNCTION(BlueprintCallable)
    void ShowCompletedTab(const TArray<class UQuestInstance*>&QuestInstances);

protected:

    // 디자이너에서 배치한 WidgetSwitcher – 탭 전환에 사용
    UPROPERTY(meta = (BindWidget))
    class UWidgetSwitcher* QuestSwitcher;

    // 위젯 전환 버튼
    UPROPERTY(meta = (BindWidget))
    class UButton* AvailableButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* InProgressButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* CompletedButton;


    // 버튼 클릭시 실행될 함수(띄울위젯)
    UFUNCTION()
    void OnAvailableButtonClicked();

    UFUNCTION()
    void OnInProgressButtonClicked();

    UFUNCTION()
    void OnCompletedButtonClicked();


    // WidgetSwitcher의 첫 번째 슬롯에 배치한 AvailableQuestWidget
    UPROPERTY(meta = (BindWidget))
    class UAvailableQuestWidget* WBP_AvailableQuest;
	
    UPROPERTY(meta = (BindWidget))
    class UInProgressQuestWidget* WBP_InProgressQuest;

    UPROPERTY(meta = (BindWidget))
    class UCompletedQuestWidget* WBP_CompletedQuest;
};
