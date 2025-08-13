// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyInfoWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GFANTASY_API UEnemyInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetName(const FText& NewName);
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EnemyName;
};
