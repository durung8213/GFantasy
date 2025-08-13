// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/ActorComponent.h"
#include "GFantasyEnums.h"
#include "GameplayTagContainer.h"
#include "DataAsset/PlayerSkillLibrary.h"
#include "SkillBarConfig.generated.h" 


/**
 * 
 */
UCLASS(BlueprintType)
class GFANTASY_API USkillBarConfig : public UDataAsset
{
	GENERATED_BODY()
	
	
public:

//	USkillBarConfig();
//


	// 슬롯 ( Q, E 등 ) -> 현재 선택된 스킬 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillBar")
	TMap<EAbilitySlot, FGameplayTag> SlotSkillTags;
	
	
};
