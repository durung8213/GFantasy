// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WerewolfPattern1UI.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UWerewolfPattern1UI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void SetFrontPercent(float Value);
};
