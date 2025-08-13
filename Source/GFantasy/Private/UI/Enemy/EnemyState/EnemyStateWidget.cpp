// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/EnemyState/EnemyStateWidget.h"
#include "GameplayEffectTypes.h"
#include "GAS/AbilitySystemComponent/GASCBase.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Enemy/EnemyState/EnemyHealthBarWidget.h"
#include "UI/Enemy/EnemyState/EnemyInfoWidget.h"
#include "UI/Enemy/EnemyState/EnemyQuestMarkWidget.h"

void UEnemyStateWidget::BindCallbackToASC(class UAbilitySystemComponent* CurrentAbilitySystem,
                                          class UGASCharacterAS* CurrentAttributeSet)
{
	if (CurrentAbilitySystem && CurrentAttributeSet)
	{
		CurrentAbilitySystem
		->GetGameplayAttributeValueChangeDelegate(
			CurrentAttributeSet->GetHealthAttribute()
		).AddUObject(this, &UEnemyStateWidget::SetHealth);

		CurrentAbilitySystem
			->GetGameplayAttributeValueChangeDelegate(
				CurrentAttributeSet->GetMaxHealthAttribute()
			).AddUObject(this, &UEnemyStateWidget::SetMaxHealth);
	
		//InitHealthBarPercent(CurrentAttributeSet);
	}
	else
	{
		if (!CurrentAbilitySystem)
		{
			//UKismetSystemLibrary::PrintString(this, TEXT("CurrentAbilitySystem 없음"), true, true, FLinearColor::Red);
		}

		if (!CurrentAttributeSet)
		{
			//UKismetSystemLibrary::PrintString(this, TEXT("CurrentAttributeSet 없음"), true, true, FLinearColor::Red);
		}
	}
	
}

// 아무것도 안보이게 함
void UEnemyStateWidget::CollapsedMode()
{
	EnemyHealthBar->SetVisibility(ESlateVisibility::Collapsed);
	EnemyInfo->SetVisibility(ESlateVisibility::Collapsed);
	QuestMarkFocus->SetVisibility(ESlateVisibility::Collapsed);
	QuestMarkUnFocus->SetVisibility(ESlateVisibility::Collapsed);
}

// 적 HpBar만 보임
void UEnemyStateWidget::UnFocusMode()
{
	EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	EnemyInfo->SetVisibility(ESlateVisibility::Collapsed);
	QuestMarkFocus->SetVisibility(ESlateVisibility::Collapsed);
	QuestMarkUnFocus->SetVisibility(ESlateVisibility::Collapsed);
}

// 적 HpBar, 퀘스트 마크가 보임
void UEnemyStateWidget::UnFocusModeWithQuest()
{
	EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	EnemyInfo->SetVisibility(ESlateVisibility::Collapsed);
	QuestMarkFocus->SetVisibility(ESlateVisibility::Collapsed);
	QuestMarkUnFocus->SetVisibility(ESlateVisibility::Visible);
}

// 적 HpBar, 이름이 보임
void UEnemyStateWidget::FocusMode()
{
	EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	EnemyInfo->SetVisibility(ESlateVisibility::Visible);
	QuestMarkFocus->SetVisibility(ESlateVisibility::Collapsed);
	QuestMarkUnFocus->SetVisibility(ESlateVisibility::Collapsed);
}

// 적 HpBar, 이름, 퀘스트 마크가 보임
void UEnemyStateWidget::FocusModeWithQuest()
{
	EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	EnemyInfo->SetVisibility(ESlateVisibility::Visible);
	QuestMarkFocus->SetVisibility(ESlateVisibility::Visible);
	QuestMarkUnFocus->SetVisibility(ESlateVisibility::Collapsed);
}


void UEnemyStateWidget::SetHealth(const FOnAttributeChangeData& Data)
{
	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetHealth(Data.NewValue);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this, Data]()
			{
				SetHealth(Data);
			}),
			0.2f,
			false
		);
	}
}

void UEnemyStateWidget::SetMaxHealth(const FOnAttributeChangeData& Data)
{
	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetMaxHealth(Data.NewValue);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this, Data]()
			{
				SetMaxHealth(Data);
			}),
			0.2f,
			false
		);
	}
	
}

void UEnemyStateWidget::InitHealthBarPercent(UGASCharacterAS* CurrentAttributeSet)
{
	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetHealth(CurrentAttributeSet->GetHealth());
		EnemyHealthBar->SetMaxHealth(CurrentAttributeSet->GetMaxHealth());
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this, CurrentAttributeSet]()
			{
				InitHealthBarPercent(CurrentAttributeSet);
			}),
			0.1f,
			false
		);
	}
}

void UEnemyStateWidget::InitHealthBarPercent(float Health, float MaxHealth)
{
	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetHealthNoRcp(Health);
		EnemyHealthBar->SetMaxHealthNoRcp(MaxHealth);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this, Health, MaxHealth]()
			{
				InitHealthBarPercent(Health, MaxHealth);
			}),
			0.1f,
			false
		);
	}
}
