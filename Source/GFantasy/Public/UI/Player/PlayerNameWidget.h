// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UPlayerNameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void SetPlayerName(const FText& InName);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerNameText;
	
	
};
