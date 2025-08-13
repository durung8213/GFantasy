// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossStateUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(BossStateUI, Log, All);

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;
class UGASCharacterAS;
class UGASBossGroggy;
struct FOnAttributeChangeData;
class UWidgetAnimation;
/**
 * 
 */
UCLASS()
class GFANTASY_API UBossStateUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void BindBossState(class UAbilitySystemComponent* CurrentBossAbilitySystem, class UGASCharacterAS* CurrentAttributeSet, class UGASBossGroggy* CurrentBossGroggy, FText BossName);

protected:

	void SetBossHealth(const FOnAttributeChangeData& Data);

public:
	void SetBossHealth2(float NewHealth);

protected:
	void SetBossMaxHealth(const FOnAttributeChangeData& Data);

public:
	void SetBossMaxHealth2(float NewMaxHealth);
	
	void SetBossGroggy(const FOnAttributeChangeData& Data);

	void SetBossGroggy2(float NewGroggy);
	
	void SetBossMaxGroggy(const FOnAttributeChangeData& Data);
	
	void SetBossMaxGroggy2(float NewMaxGroggy);

	void SetBossName(FText NewBossName) const;
	
	UFUNCTION()
	void SetBossHpBarPercent();
	
	UFUNCTION()
	void SetBossGroggyBarPercent();

public:
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossNameTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossHpProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossGroggyProgressBar;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* BossHitShake;
	

protected:
	float BossMaxHealth = 100;
	float BossHealth = 0;
	float BossMaxGroggy = 100;
	float BossGroggy = 100;
};
