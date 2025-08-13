// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseQuestStateWidget.h"
#include "CompletedQuestWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UCompletedQuestWidget : public UBaseQuestStateWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Quest|UI")
	void PopulateCompletedQuests(const TArray<class UQuestInstance*>& QuestInstances);

protected:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* QuestScrollBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UQuestWidget> QuestWidgetClass;

private:
	void CheckQuestInstance(class UQuestInstance* QI);
};
