// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataAsset/SkillBarConfig.h"
#include "AbilitySystemComponent.h"
#include "Character/SkillBar/SkillBarComponent.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "CoolDownWidget.generated.h"

/**
 * 
 */



UCLASS()
class GFANTASY_API UCoolDownWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Cooldown")
	void HandleCooldownStart(FGameplayTag CooldownTag, FActiveGameplayEffectHandle Handle,
		float StartTime, float Duration);
	
public:
	// Q, E 등 현재 슬롯
	UPROPERTY(BlueprintReadWrite)
	EAbilitySlot SkillSlot;
	
	// Owner ASC
	UPROPERTY(BlueprintReadWrite)
	UPlayerCharacterGASC* PlayerASC;

	// 캐릭터 스킬 바
	UPROPERTY(BlueprintReadWrite)
	USkillBarComponent* SkillBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ratio")
	float ProgressRatio = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
	FGameplayTag EffectCooldownTag;
	
private:
	FActiveGameplayEffectHandle TrackedCooldownHandle;

	float CooldownStart = 0.0f;
	
	float CooldownDuration = 0.0f;

	bool bIsCoolDownActive = false;
};
