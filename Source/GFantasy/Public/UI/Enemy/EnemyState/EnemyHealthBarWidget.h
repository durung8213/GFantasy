// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class GFANTASY_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
public:
	void SetHealth(float NewHealth);

	void SetHealthNoRcp(float NewHealth);
	
	void SetMaxHealth(float NewMaxHelath);

	void SetMaxHealthNoRcp(float NewHealth);

	void SetProgressBarPercent() const;

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBarBack;

protected:
	void SetBackProgressBarPercent() const;

	void LerpBackProgressBarStart();
	
	void LerpBackProgressBarPercent();
	
	void ChangeHpbarColor();
	
protected:
	FTimerHandle HealthHitTimer;
	FTimerHandle HealthHitLerpTimer; 
	
	float Health = 100;
	float MaxHealth = 100;

	float HealthBack = 100;
	float OldHealthBack = 100;
	float MaxHealthBack = 100;

	float CurrentElapsedTime=0.f;

	float TickTime = 0.3333f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LerpTime = 0.3f;

	FProgressBarStyle NormalProgressBarStyle;
	FProgressBarStyle HitProgressBarStyle;
};
