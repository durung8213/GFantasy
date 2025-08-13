// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCTypeWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UNPCTypeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void SetNPCTypeName(const FText& InName);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TypeText;
	
	
	
};
