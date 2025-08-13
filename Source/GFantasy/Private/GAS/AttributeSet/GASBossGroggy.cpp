// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AttributeSet/GASBossGroggy.h"
#include "GAS/GamePlayTag.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


void UGASBossGroggy::OnRep_Groggy(const FGameplayAttributeData& OldGroggy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASBossGroggy, Groggy, OldGroggy);
}

void UGASBossGroggy::OnRep_MaxGroggy(const FGameplayAttributeData& OldMaxGroggy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASBossGroggy, MaxGroggy, OldMaxGroggy);
}

void UGASBossGroggy::OnRep_ExtraGauge(const FGameplayAttributeData& OldExtraGauge)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASBossGroggy, ExtraGauge, OldExtraGauge);
}

void UGASBossGroggy::OnRep_MaxExtraGauge(const FGameplayAttributeData& OldMaxExtraGauge)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASBossGroggy, MaxExtraGauge, OldMaxExtraGauge);
}

void UGASBossGroggy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UGASBossGroggy, Groggy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASBossGroggy, MaxGroggy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASBossGroggy, ExtraGauge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASBossGroggy, MaxExtraGauge, COND_None, REPNOTIFY_Always);
}

void UGASBossGroggy::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetGroggyAttribute()) {	// 바뀐 값이 Health면
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxGroggy());	// 이 값을 NewValue로 해라
	}

	if (Attribute == GetExtraGaugeAttribute()) {	// 바뀐 값이 Health면
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxExtraGauge());	// 이 값을 NewValue로 해라
	}
	
}

void UGASBossGroggy::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetGroggyAttribute()) {
		SetGroggy(FMath::Clamp(GetGroggy(), 0.f, GetMaxGroggy()));	// 다시 Clamp했다

		if (GetGroggy() == GetMaxGroggy())
		{
			OnGroggyChargeMax.Broadcast();
		}

		if (GetGroggy() == 0)
		{
			OnGroggyUseUp.Broadcast();
		}

		if (GetExtraGauge() == GetMaxExtraGauge())
		{
			OnExtraGaugeFull.Broadcast();
			FGameplayEventData EventData;
			EventData.EventTag = FGameplayTags::Get().Enemy_Boss_ExtraGaugeFull;
			//EventData.Instigator = MeshComp->GetOwner();
			//EventData.Target = MeshComp->GetOwner();

			GetOwningAbilitySystemComponent()->HandleGameplayEvent(FGameplayTags::Get().Enemy_Boss_ExtraGaugeFull, &EventData);
		}
	}

	if (Data.EvaluatedData.Attribute == GetExtraGaugeAttribute()) {
		SetExtraGauge(FMath::Clamp(GetExtraGauge(), 0.f, GetMaxExtraGauge()));	// 다시 Clamp했다

		if (GetExtraGauge() == GetMaxExtraGauge())
		{
			OnExtraGaugeFull.Broadcast();
			FGameplayEventData EventData;
			EventData.EventTag = FGameplayTags::Get().Enemy_Boss_ExtraGaugeFull;
			//EventData.Instigator = MeshComp->GetOwner();
			//EventData.Target = MeshComp->GetOwner();

			GetOwningAbilitySystemComponent()->HandleGameplayEvent(FGameplayTags::Get().Enemy_Boss_ExtraGaugeFull, &EventData);
		}
	}
}
