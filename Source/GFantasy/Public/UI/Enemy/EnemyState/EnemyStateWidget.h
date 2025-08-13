// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "EnemyStateWidget.generated.h"


class UAbilitySystemComponent;
class UAttributeSet;
struct FOnAttributeChangeData;
class UEnemyQuestMarkWidget;
class UEnemyHealthBarWidget;
class UEnemyInfoWidget;
class UGASCharacterAS;
class UGASCBase;

/**
 * 
 */
UCLASS()
class GFANTASY_API UEnemyStateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindCallbackToASC(UAbilitySystemComponent* CurrentAbilitySystem, UGASCharacterAS* CurrentAttributeSet);

	// 아무것도 안보이게 함
	void CollapsedMode();

	// 적 HpBar만 보임
	void UnFocusMode();

	// 적 HpBar, 퀘스트 마크가 보임
	void UnFocusModeWithQuest();

	// 적 HpBar, 이름이 보임
	void FocusMode();

	// 적 HpBar, 이름, 퀘스트 마크가 보임
	void FocusModeWithQuest();
	
protected:
	void SetHealth(const FOnAttributeChangeData& Data);
	
	void SetMaxHealth(const FOnAttributeChangeData& Data);

public:
	void InitHealthBarPercent(UGASCharacterAS* CurrentAttributeSet);

	void InitHealthBarPercent(float Health, float MaxHealth);
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEnemyHealthBarWidget> EnemyHealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEnemyInfoWidget> EnemyInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEnemyQuestMarkWidget> QuestMarkFocus;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEnemyQuestMarkWidget> QuestMarkUnFocus;

	
};
