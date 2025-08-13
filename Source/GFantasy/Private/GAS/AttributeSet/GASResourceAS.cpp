// Fill out your copyright notice in the Description page of Project Settings.
#include "GAS/AttributeSet/GASResourceAS.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"

void UGASResourceAS::OnRep_LimitGage(const FGameplayAttributeData& OldLimitGage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASResourceAS, LimitGage, OldLimitGage);
}

void UGASResourceAS::OnRep_MaxLimitGage(const FGameplayAttributeData& OldMaxLimitGage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASResourceAS, MaxLimitGage, OldMaxLimitGage);
}

void UGASResourceAS::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Limit Gage 의 값이 적용되기 전에 미리 클램프를 적용.
	if (Attribute == GetLimitGageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxLimitGage());
	}

}

void UGASResourceAS::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Limit Gage 의 값이 변경되었을 때, 해당 값을 클램프하고, 변경된 값을 이벤트로 전달.
	if (Data.EvaluatedData.Attribute == GetLimitGageAttribute())
	{
		SetLimitGage(FMath::Clamp(GetLimitGage(), 0.f, GetMaxLimitGage()));
		
		const float NewValue = GetLimitGage();
		if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
		{
			if (UPlayerCharacterGASC* PlayerASC = Cast<UPlayerCharacterGASC>(ASC))
			{
				PlayerASC->OnLimitGageChanged.ExecuteIfBound(NewValue);
			}
		}
	}
}

void UGASResourceAS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASResourceAS, LimitGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASResourceAS, MaxLimitGage, COND_None, REPNOTIFY_Always);

} 
