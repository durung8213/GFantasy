// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPC/NPCNameWidget.h"
#include "Components/TextBlock.h"

void UNPCNameWidget::SetNPCName(const FText& InName)
{
	if (NameText)
	{
		NameText->SetText(InName);
	}
}
