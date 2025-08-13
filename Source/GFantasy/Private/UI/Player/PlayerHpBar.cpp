// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/PlayerHpBar.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "GameplayEffectTypes.h"

void UPlayerHpBar::InitializeWithASC(UAbilitySystemComponent* InASC, UGASCharacterAS* InAS)
{
	ASC = InASC;
	AS = InAS;

	if(!ASC.IsValid() || !AS.IsValid())
		return;

	CachedMaxHealth = AS->GetMaxHealth();

	// ��������Ʈ ���� HP
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).
		AddUObject(this, &UPlayerHpBar::OnHealthChanged);

	// ��������Ʈ ���� MaxHp
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).
		AddUObject(this, &UPlayerHpBar::OnMaxHealthChanged);

	FOnAttributeChangeData DummyData;
	DummyData.NewValue = AS->GetHealth();

	// �ʱ� ǥ��
	OnHealthChanged(DummyData);
	

}

void UPlayerHpBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerHpBar::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	// �÷��̾� ���
	if (CachedMaxHealth <= 0.f)
		return;

	float Percent = Data.NewValue / CachedMaxHealth;

	Percent = FMath::Clamp(Percent, 0.0f, 1.0f);

	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UPlayerHpBar::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CachedMaxHealth = Data.NewValue;

	FOnAttributeChangeData DummyData;
	DummyData.NewValue = AS->GetHealth();

	OnHealthChanged(DummyData);
}
