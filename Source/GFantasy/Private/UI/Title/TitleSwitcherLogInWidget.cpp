// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/TitleSwitcherLogInWidget.h"
#include "Components/Button.h"

void UTitleSwitcherLogInWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &UTitleSwitcherLogInWidget::OnLogInButtonClicked);
		LoginButton->OnClicked.AddDynamic(this, &UTitleSwitcherLogInWidget::HandleLoginButtonClicked);
	}
}

void UTitleSwitcherLogInWidget::OnLogInButtonClicked()
{
    UWidget* Root = GetRootWidget();
    if (Root)
    {
        Root->RemoveFromParent();
    }
}

void UTitleSwitcherLogInWidget::HandleLoginButtonClicked()
{
	OnLoginRequested.Broadcast();
}
