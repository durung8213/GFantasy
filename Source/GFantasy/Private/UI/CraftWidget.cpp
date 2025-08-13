// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CraftWidget.h"


void UCraftWidget::SetWidgetController(UWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	OnWidgetControllerSet();
}
