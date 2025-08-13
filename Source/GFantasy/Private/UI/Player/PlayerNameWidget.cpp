// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerNameWidget.h"
#include "Components/TextBlock.h"

void UPlayerNameWidget::SetPlayerName(const FText& InName)
{
	if (PlayerNameText)
	{
		PlayerNameText->SetText(InName);
	}
}
