// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossExtraGaugeUserWidget.generated.h"

/**
 * 
 */

class UProgressBar;
class UImage;

UCLASS()
class GFANTASY_API UBossExtraGaugeUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SpacebarClick();

	void SetExtraGaugeValue(float NewExtraGaugeValue);

	void SetMaxExtraGaugeValue(float NewMaxExtraGaugeValue);

	void SetExtraGaugePercent() const;

protected:
	void ResetSpacebarImage();

public:
	UPROPERTY(meta = (BindWidget))
	UImage* SpacebarImage;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExtraGaugeProgressBar;

protected:
	float ExtraGaugeValue = 0;
	float MaxExtraGaugeValue = 100;

	bool bIsButtonClicked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossExtraGauge")
	float ResetButtonTime = 0.2f;
};
