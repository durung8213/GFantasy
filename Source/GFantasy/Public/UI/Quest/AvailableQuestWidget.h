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
	// ���÷� �־������ �����
	virtual void NativeConstruct() override; //Begin play������ �Ѵ�.

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PopulateAvailableQuests(const TArray<UQuestInstance*>& QuestInstances);

protected:
    // WBP_AvailableQuest �������Ʈ���� ���ε��� ScrollBox ����
    UPROPERTY(meta = (BindWidget))
    UScrollBox* QuestScrollBox;

    // (������ �Ҵ�) ������ WBP_Quest(UserWidget) Ŭ����
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UQuestWidget> QuestWidgetClass;

private:
    void CheckQuestInstance(UQuestInstance* QI);
};
