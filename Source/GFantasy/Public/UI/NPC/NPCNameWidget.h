// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UNPCNameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void SetNPCName(const FText& InName);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NameText;
	
	
};
