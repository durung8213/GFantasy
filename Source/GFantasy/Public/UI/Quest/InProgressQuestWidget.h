// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseQuestStateWidget.h"
#include "InProgressQuestWidget.generated.h"

/**
 * 
 */
class UScrollBox;
class UQuestWidget;
class UQuestInstance;

UCLASS()
class GFANTASY_API UInProgressQuestWidget : public UBaseQuestStateWidget
{
	GENERATED_BODY()
	
public:
    // Begin play역할
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Quest|UI")
    void PopulateInProgressQuests(const TArray<UQuestInstance*>& QuestInstances);

protected:
    // WBP_AvailableQuest 블루프린트에서 바인딩한 ScrollBox 위젯
    UPROPERTY(meta = (BindWidget))
    UScrollBox* QuestScrollBox;

    // (에디터 할당) 생성할 WBP_Quest(UserWidget) 클래스
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest|UI")
    TSubclassOf<UQuestWidget> QuestWidgetClass;
	
private:
    void CheckQuestInstance(UQuestInstance* QI);
};
