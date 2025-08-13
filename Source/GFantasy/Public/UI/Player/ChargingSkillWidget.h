// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "ChargingSkillWidget.generated.h"



class UPlayerCharacterGASC;

UCLASS()
class GFANTASY_API UChargingSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	 void BindToASC(UPlayerCharacterGASC* ASC);

	UFUNCTION()
	 void HandleUIChanged(FGameplayTag Tag, bool bAdded);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCharagetSkillAvilabilityChanged(bool bUsable);
	
};
