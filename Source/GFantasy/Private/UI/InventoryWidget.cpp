// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryWidget.h"

void UInventoryWidget::SetWidgetController(UWidgetController* InWidgetController)
{
	WidgetController = InWidgetController;
	OnWidgetControllerSet();
}
