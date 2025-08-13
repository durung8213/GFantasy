// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPC/NPCTypeWidget.h"
#include "Components/TextBlock.h"

void UNPCTypeWidget::SetNPCTypeName(const FText& InName)
{
	if (TypeText)
	{
		TypeText->SetText(InName);
	}
}
