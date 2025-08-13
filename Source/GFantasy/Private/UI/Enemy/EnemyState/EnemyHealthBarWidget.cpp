// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/EnemyState/EnemyHealthBarWidget.h"

#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


void UEnemyHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (HealthProgressBar)
	{
		// NormalProgressBarStyle = HealthProgressBar->GetWidgetStyle();
		// HitProgressBarStyle = HealthProgressBar->GetWidgetStyle();
		// HitProgressBarStyle.FillImage.TintColor = FSlateColor(FLinearColor(1, 0, 0.007763f));
		TickTime = GetWorld()->GetDeltaSeconds();
	}
}

void UEnemyHealthBarWidget::SetHealth(float NewHealth)
{

	if (NewHealth == 0 && Health != NewHealth)
	{
		Health = NewHealth;
		SetProgressBarPercent();
		GetWorld()->GetTimerManager().ClearTimer(HealthHitTimer);
		GetWorld()->GetTimerManager().ClearTimer(HealthHitLerpTimer);
		LerpBackProgressBarStart();
	}
	
	if (NewHealth == 0 || Health == NewHealth)
	{
		return;
	}
	
	if (Health == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(HealthHitTimer);
		GetWorld()->GetTimerManager().ClearTimer(HealthHitLerpTimer);
		LerpBackProgressBarStart();
	}
	else if (Health > NewHealth)
	{
		if (HealthProgressBar)
		{
			//HealthProgressBar->SetWidgetStyle(HitProgressBarStyle);
			OldHealthBack = HealthBack;
			GetWorld()->GetTimerManager().ClearTimer(HealthHitTimer);
			GetWorld()->GetTimerManager().ClearTimer(HealthHitLerpTimer);
			GetWorld()->GetTimerManager().SetTimer(
				HealthHitTimer,
				this,
				&UEnemyHealthBarWidget::LerpBackProgressBarStart,
				1.f,
				false
			);
		}
	}
	else
	{
		OldHealthBack= NewHealth;
		HealthBack = NewHealth;
		SetBackProgressBarPercent();
	}
	
	Health = NewHealth;
	SetProgressBarPercent();
}

void UEnemyHealthBarWidget::SetHealthNoRcp(float NewHealth)
{
	Health = NewHealth;
	OldHealthBack = NewHealth;
	HealthBack = NewHealth;
	SetProgressBarPercent();
	SetBackProgressBarPercent();
}

void UEnemyHealthBarWidget::SetMaxHealth(float NewMaxHelath)
{
	MaxHealth = NewMaxHelath;
	MaxHealthBack = NewMaxHelath;
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("SetMaxHealth : %f"), MaxHealth));
	SetProgressBarPercent();
	SetBackProgressBarPercent();
}

void UEnemyHealthBarWidget::SetMaxHealthNoRcp(float NewHealth)
{
	MaxHealth = NewHealth;
	MaxHealthBack = NewHealth;
}

void UEnemyHealthBarWidget::SetProgressBarPercent() const
{
	float Percent = FMath::Clamp(Health / MaxHealth, 0.f, 1.f);
	
	HealthProgressBar->SetPercent(Percent);
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Health : %f, MaxHealth : %f"), Health, MaxHealth));
}

void UEnemyHealthBarWidget::SetBackProgressBarPercent() const
{
	float Percent = FMath::Clamp(HealthBack / MaxHealthBack, 0.f, 1.f);
	HealthProgressBarBack->SetPercent(Percent);
}

void UEnemyHealthBarWidget::LerpBackProgressBarStart()
{
	CurrentElapsedTime=0.f;
	
	GetWorld()->GetTimerManager().SetTimer(
		HealthHitLerpTimer,
		this,
		&UEnemyHealthBarWidget::LerpBackProgressBarPercent,
		GetWorld()->GetDeltaSeconds(),
		true
	);
}

void UEnemyHealthBarWidget::LerpBackProgressBarPercent()
{
	CurrentElapsedTime+=GetWorld()->GetDeltaSeconds();

	float LerpPercent = CurrentElapsedTime / LerpTime;
	HealthBack =FMath::Lerp(OldHealthBack,Health ,LerpPercent);
	
	//HealthBack = UKismetMathLibrary::FInterpTo(OldHealthBack,Health , GetWorld()->GetDeltaSeconds(), 0.1f);
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Health : %lf, HealthBack ; %lf, OldHealthBack : %lf, LerpPercent : %lf"), Health, HealthBack, OldHealthBack, LerpPercent));
	SetBackProgressBarPercent();
	
	if (LerpPercent >= 1.f)
	{
		HealthBack= Health;
		OldHealthBack = Health;
		GetWorld()->GetTimerManager().ClearTimer(HealthHitLerpTimer);
	}
}

void UEnemyHealthBarWidget::ChangeHpbarColor()
{
	//HealthProgressBar->SetWidgetStyle(NormalProgressBarStyle);
}
