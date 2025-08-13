// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Title/TitleLogInWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Title/TitleSwitcherLogInWidget.h"
//#include "UI/TitleWidget.h"

void UTitleLogInWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &UTitleLogInWidget::OnLogInButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UTitleLogInWidget::OnJoinButtonClicked);
	}

	if (WBP_TitleSwitcherLogIn)
	{
		WBP_TitleSwitcherLogIn->OnLoginRequested.AddDynamic(this, &UTitleLogInWidget::HandleLoginRequested);
	}
}

void UTitleLogInWidget::OnLogInButtonClicked()
{
	LogInSwitcher->SetActiveWidgetIndex(0);
}

void UTitleLogInWidget::OnJoinButtonClicked()
{
	LogInSwitcher->SetActiveWidgetIndex(1);
}

void UTitleLogInWidget::HandleLoginRequested()
{
	RemoveFromParent();

	//// 다음 위젯 생성해서 Viewport 에 추가
	//if (TitleWidgetClass && GetOwningPlayer())
	//{
	//	UUserWidget* TitleWidget = CreateWidget<UTitleWidget>(GetOwningPlayer(), TitleWidgetClass);
	//	if (TitleWidget)
	//	{
	//		TitleWidget->AddToViewport();
	//	}
	//}
}
