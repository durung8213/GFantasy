// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Quest/QuestMainWidget.h"
#include "Components/VerticalBox.h"
#include "UI/Quest/QuestForMainWidget.h"

void UQuestMainWidget::ClearQuestList()
{
	if (InProgressQuestList)
	{
		InProgressQuestList->ClearChildren();
	}
}

void UQuestMainWidget::ShowInProgressQuestList(const TArray<UQuestInstance*>& NewInProgressArr)
{
	if (InProgressQuestList)
	{
		InProgressQuestList->ClearChildren();

		for (UQuestInstance* Quest : NewInProgressArr)
		{
			if (Quest)
			{
				UQuestForMainWidget* QuestForMainWidget = CreateWidget<UQuestForMainWidget>(GetOwningPlayer(), QuestWidgetClass);
				QuestForMainWidget->InitializeWidget(Quest);
				InProgressQuestList->AddChild(QuestForMainWidget);
			}
		}
	}
}
