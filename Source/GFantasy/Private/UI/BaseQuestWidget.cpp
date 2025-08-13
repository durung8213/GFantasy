// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BaseQuestWidget.h"

void UBaseQuestWidget::InitializeQuestData(const TArray<UQuestInstance*>& InAllQuests, const TArray<UQuestInstance*>& InAvailable, const TArray<UQuestInstance*>& InInProgress, const TArray<UQuestInstance*>& InCompleted)
{
}

void UBaseQuestWidget::SetAvailableArr(TArray<UQuestInstance*>& InArr)
{
	AvailableArr = InArr;
}

void UBaseQuestWidget::SetInProgressArr(TArray<UQuestInstance*>& InArr)
{
	InProgressArr = InArr;
}

void UBaseQuestWidget::SetCompletedArr(TArray<UQuestInstance*>& InArr)
{
	CompletedArr = InArr;
}

