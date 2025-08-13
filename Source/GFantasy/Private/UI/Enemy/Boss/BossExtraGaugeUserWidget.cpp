// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/Boss/BossExtraGaugeUserWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UBossExtraGaugeUserWidget::SpacebarClick()
{
	if (!bIsButtonClicked)
	{
		FWidgetTransform Change = SpacebarImage->GetRenderTransform();
		Change.Scale = FVector2D(1.0f, 1.0f);
		SpacebarImage->SetRenderTransform(Change);
		bIsButtonClicked = true;

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&UBossExtraGaugeUserWidget::ResetSpacebarImage,
			ResetButtonTime,
			false
		);
	}
	
}

void UBossExtraGaugeUserWidget::SetExtraGaugeValue(const float NewExtraGaugeValue)
{
	ExtraGaugeValue = NewExtraGaugeValue;
	SetExtraGaugePercent();
}

void UBossExtraGaugeUserWidget::SetMaxExtraGaugeValue(const float NewMaxExtraGaugeValue)
{
	MaxExtraGaugeValue = NewMaxExtraGaugeValue;
	SetExtraGaugePercent();
}

void UBossExtraGaugeUserWidget::SetExtraGaugePercent() const
{
	float Percent = FMath::Clamp(ExtraGaugeValue/MaxExtraGaugeValue, 0, 1);
	ExtraGaugeProgressBar->SetPercent(Percent);
}

void UBossExtraGaugeUserWidget::ResetSpacebarImage()
{
	FWidgetTransform Change = SpacebarImage->GetRenderTransform();
	Change.Scale = FVector2D(0.9f, 0.9f);
	SpacebarImage->SetRenderTransform(Change);
	bIsButtonClicked = false;
}
