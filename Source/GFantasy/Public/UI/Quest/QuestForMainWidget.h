// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseUserQuestWidget.h"
#include "QuestForMainWidget.generated.h"

class UTextBlock;
class UQuestInstance;

UCLASS()
class GFANTASY_API UQuestForMainWidget : public UBaseUserQuestWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // QuestInstance  표시할 퀘스트 인스턴스
    UFUNCTION(BlueprintCallable, Category = "Quest|UI")
    void InitializeWidget(UQuestInstance* QuestInstance);

    void SetQuestName(UQuestInstance* QuestInstance);
    void SetQuestDescription(UQuestInstance* QuestInstance);
    void SetQuestState(UQuestInstance* QuestInstance);
    void SetCurrentCount(UQuestInstance* QuestInstance);
    void SetRequiredCount(UQuestInstance* QuestInstance);
    void SetProgressPercent(UQuestInstance* QuestInstance);
    void SetCheckBox(UQuestInstance* QuestInstance);

private:
    // 태그의 마지막 .뒤의값만 보여주는 함수
    FString GetStateNameFromTag(const FString& FullTag) const;

private:
    UPROPERTY()
    UQuestInstance* CurrentQuestInstance = nullptr;


public:
    // 퀘스트 이름을 표시하는 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestName;

    // 퀘스트 설명을 표시하는 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestDescription;

    // 퀘스트 상태(태그)를 표시하는 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestState;

    // 현재 수집/처치 개수를 표시하는 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentCount;

    // 요구 수량을 표시하는 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* RequiredCount;

    // 진행률(%)을 표시하는 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* ProgressPercent;
};
