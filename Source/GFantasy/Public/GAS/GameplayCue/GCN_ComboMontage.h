// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "DataAsset/GASAbilityBase.h"
#include "GCN_ComboMontage.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew, Blueprintable)
class GFANTASY_API UGCN_ComboMontage : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	//큐가 실행되면 해당 함수가 호출됩니다.
	virtual bool OnActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters
	) const override;
	

	
};
