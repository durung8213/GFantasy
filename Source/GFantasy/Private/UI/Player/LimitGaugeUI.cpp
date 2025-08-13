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

	// 델리게이트 연결 
	ASC->GetGameplayAttributeValueChangeDelegate(AS->GetLimitGageAttribute()).
		AddUObject(this, &ULimitGaugeUI::OnLimitGaugeChanged);

	FOnAttributeChangeData DummyData;
	DummyData.NewValue = AS->GetLimitGage();

	// 초기 표시
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
		
		// MAX 도달 시 색상 변경
		if (Percent >= 1.0f)
		{
			LimitGaugeBar->SetFillColorAndOpacity(FLinearColor(1.0f,0.5f,0.0f));

			// 효과음 추가
		}
		else
		{
			LimitGaugeBar->SetFillColorAndOpacity(FLinearColor::Yellow);
		}
	}


}
