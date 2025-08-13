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
    // Begin play����
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Quest|UI")
    void PopulateInProgressQuests(const TArray<UQuestInstance*>& QuestInstances);

protected:
    // WBP_AvailableQuest �������Ʈ���� ���ε��� ScrollBox ����
    UPROPERTY(meta = (BindWidget))
    UScrollBox* QuestScrollBox;

    // (������ �Ҵ�) ������ WBP_Quest(UserWidget) Ŭ����
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest|UI")
    TSubclassOf<UQuestWidget> QuestWidgetClass;
	
private:
    void CheckQuestInstance(UQuestInstance* QI);
};
