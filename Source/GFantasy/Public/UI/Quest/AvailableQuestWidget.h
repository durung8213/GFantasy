// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseQuestStateWidget.h"
#include "AvailableQuestWidget.generated.h"

/**
 * 
 */

class UScrollBox;
class UQuestWidget;
class UQuestInstance;

UCLASS()
class GFANTASY_API UAvailableQuestWidget : public UBaseQuestStateWidget
{
	GENERATED_BODY()
	
public:	
	// 샘플로 넣어놓을것 지우기
	virtual void NativeConstruct() override; //Begin play역할을 한다.

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PopulateAvailableQuests(const TArray<UQuestInstance*>& QuestInstances);

protected:
    // WBP_AvailableQuest 블루프린트에서 바인딩한 ScrollBox 위젯
    UPROPERTY(meta = (BindWidget))
    UScrollBox* QuestScrollBox;

    // (에디터 할당) 생성할 WBP_Quest(UserWidget) 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UQuestWidget> QuestWidgetClass;

private:
    void CheckQuestInstance(UQuestInstance* QI);
};
