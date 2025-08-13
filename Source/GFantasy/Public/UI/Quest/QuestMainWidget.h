// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Quest/BaseUserQuestWidget.h"
#include "QuestMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UQuestMainWidget : public UBaseUserQuestWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* InProgressQuestList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	TSubclassOf<class UQuestForMainWidget> QuestWidgetClass;

public:
	// InProgressQuestList 안의 샘플 위젯을 제거 함수
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void ClearQuestList();

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void ShowInProgressQuestList(const TArray<UQuestInstance*>& NewInProgressArr);
	
};
