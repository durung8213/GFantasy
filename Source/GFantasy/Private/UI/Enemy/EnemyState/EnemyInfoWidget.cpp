// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/EnemyState/EnemyInfoWidget.h"

#include "Components/TextBlock.h"

void UEnemyInfoWidget::SetName(const FText& NewName)
{
	EnemyName->SetText(NewName);
}
