// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/LimitGaugeUI.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GAS/AttributeSet/GASResourceAS.h"



void ULimitGaugeUI::InitializeWithASC(UAbilitySystemComponent* InASC, UGASResourceAS* InAS)
{
	ASC = InASC;
	AS = InAS;

	if (!ASC.IsValid() || !AS.IsValid())
		return;

	CachedMaxLimitGauge = AS->GetMaxLimitGage();

	// ��������Ʈ ���� 
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetLimitGageAttribute()).
		AddUObject(this, &ULimitGaugeUI::OnLimitGaugeChanged);

	FOnAttributeChangeData DummyData;
	DummyData.NewValue = AS->GetLimitGage();

	// �ʱ� ǥ��
	OnLimitGaugeChanged(DummyData);

}

void ULimitGaugeUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULimitGaugeUI::OnLimitGaugeChanged(const FOnAttributeChangeData& Data)
{
	float Percent = Data.NewValue / CachedMaxLimitGauge;

	Percent = FMath::Clamp(Percent, 0.0f, 1.0f);

	if (LimitGaugeBar)
	{
		LimitGaugeBar->SetPercent(Percent);	
		
		// MAX ���� �� ���� ����
		if (Percent >= 1.0f)
		{
			LimitGaugeBar->SetFillColorAndOpacity(FLinearColor(1.0f,0.5f,0.0f));

			// ȿ���� �߰�
		}
		else
		{
			LimitGaugeBar->SetFillColorAndOpacity(FLinearColor::Yellow);
		}
	}


}
