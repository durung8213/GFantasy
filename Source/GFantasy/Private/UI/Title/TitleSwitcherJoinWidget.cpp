// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/TitleSwitcherJoinWidget.h"
#include "Components/Button.h"

void UTitleSwitcherJoinWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CheckButton)
	{
		CheckButton->OnClicked.AddDynamic(this, &UTitleSwitcherJoinWidget::OnCheckButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UTitleSwitcherJoinWidget::OnJoinButtonClicked);
	}
}

void UTitleSwitcherJoinWidget::OnCheckButtonClicked()
{
}

void UTitleSwitcherJoinWidget::OnJoinButtonClicked()
{
}
