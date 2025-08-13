// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/Boss/BossStateUI.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GAS/AttributeSet/GASBossGroggy.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GameplayEffectTypes.h"
#include "GAS/AbilitySystemComponent/GASCBase.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(BossStateUI);

void UBossStateUI::BindBossState(UAbilitySystemComponent* CurrentBossAbilitySystem, UGASCharacterAS* CurrentAttributeSet, UGASBossGroggy* CurrentBossGroggy, FText BossName)
{
	if (!CurrentBossAbilitySystem) return;
		
	if (!CurrentAttributeSet) return;

	if (!CurrentBossGroggy) return;

	CurrentBossAbilitySystem->GetGameplayAttributeValueChangeDelegate(
			CurrentAttributeSet->GetHealthAttribute()
		).AddUObject(this, &UBossStateUI::SetBossHealth);

	CurrentBossAbilitySystem->GetGameplayAttributeValueChangeDelegate(
			CurrentAttributeSet->GetHealthAttribute()
		).AddUObject(this, &UBossStateUI::SetBossMaxHealth);

	CurrentBossAbilitySystem->GetGameplayAttributeValueChangeDelegate(
		CurrentBossGroggy->GetGroggyAttribute()
	).AddUObject(this, &UBossStateUI::SetBossGroggy);

	CurrentBossAbilitySystem->GetGameplayAttributeValueChangeDelegate(
		CurrentBossGroggy->GetMaxGroggyAttribute()
	).AddUObject(this, &UBossStateUI::SetBossMaxGroggy);

	if (BossNameTextBlock)
	{
		BossNameTextBlock->SetText(BossName);
	}
	
}

void UBossStateUI::SetBossHealth(const FOnAttributeChangeData& Data)
{
	BossHealth = Data.NewValue;
	
	SetBossHpBarPercent();
}

void UBossStateUI::SetBossHealth2(float NewHealth)
{
	if (BossHealth >= NewHealth)
	{
		PlayAnimation(BossHitShake);
	}
	BossHealth = NewHealth;
	SetBossHpBarPercent();
}

void UBossStateUI::SetBossMaxHealth2(float NewMaxHealth)
{
	BossMaxHealth = NewMaxHealth;
	SetBossHpBarPercent();
}

void UBossStateUI::SetBossMaxHealth(const FOnAttributeChangeData& Data)
{
	BossMaxHealth = Data.NewValue;
	SetBossHpBarPercent();
}

void UBossStateUI::SetBossGroggy(const FOnAttributeChangeData& Data)
{
	BossGroggy = Data.NewValue;
	SetBossGroggyBarPercent();
}

void UBossStateUI::SetBossGroggy2(float NewGroggy)
{
	BossGroggy = NewGroggy;
	SetBossGroggyBarPercent();
}


void UBossStateUI::SetBossMaxGroggy(const FOnAttributeChangeData& Data)
{
	BossMaxGroggy = Data.NewValue;
	SetBossGroggyBarPercent();
}

void UBossStateUI::SetBossMaxGroggy2(float NewMaxGroggy)
{
	BossMaxGroggy = NewMaxGroggy;
	SetBossGroggyBarPercent();
}

void UBossStateUI::SetBossName(FText NewBossName) const
{
	if (BossNameTextBlock)
	{
		BossNameTextBlock->SetText(NewBossName);
	}
	
}


void UBossStateUI::SetBossHpBarPercent()
{
	float CurrentPercent = FMath::Clamp(BossHealth/BossMaxHealth, 0, 1);
	if (BossHpProgressBar)
	{
		BossHpProgressBar->SetPercent(CurrentPercent);
		UE_LOG(BossStateUI, Error, TEXT("Health Percent : %lf"), CurrentPercent);
	}
	else
	{
		UE_LOG(BossStateUI, Error, TEXT("BossHpProgressBar is not valid"));
	}
}

void UBossStateUI::SetBossGroggyBarPercent()
{
	float CurrentPercent = FMath::Clamp(BossGroggy/BossMaxGroggy, 0, 1);
	if (BossGroggyProgressBar)
	{
		BossGroggyProgressBar->SetPercent(CurrentPercent);
		UE_LOG(BossStateUI, Error, TEXT("Groggy Percent : %lf"), CurrentPercent);
	}
	else
	{
		UE_LOG(BossStateUI, Error, TEXT("BossGroggyProgressBar is not valid"));
	}
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Groggy : %f, MaxGroggy : %f"), BossGroggy, BossMaxGroggy));
}
