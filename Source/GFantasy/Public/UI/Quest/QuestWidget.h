// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseUserQuestWidget.h"
#include "QuestWidget.generated.h"

 // 위젯에 바인드할 TextBlock 전방 선언
class UTextBlock;
class UQuestInstance;

// 체크박스 변경 시 브로드캐스트할 델리게이트: 체크 여부, 퀘스트 인스턴스 전달
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTraceCheckChanged, UQuestInstance*, QuestInstance, bool, bChecked);

UCLASS()
class GFANTASY_API UQuestWidget : public UBaseUserQuestWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    // QuestInstance와 델리게이트 바인딩
    UFUNCTION(BlueprintCallable, Category = "Quest|UI")
    void BindToQuest(UQuestInstance* QuestInstance);

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

    // 델리게이트: 체크박스 토글 시 호출
    UPROPERTY(BlueprintAssignable, Category = "Quest|Indicator")
    FOnTraceCheckChanged OnTraceCheckChanged;

    // 체크박스 상태변경 핸들러
    UFUNCTION()
    void HandleTraceCheckChanged(bool bIsChecked);


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


    UPROPERTY(meta = (BindWidget))
    class UCheckBox* TraceOnOffCheck;
	
   };
