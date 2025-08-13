// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/BaseHUD.h"
#include "GameplayTagContainer.h"
#include "GFantasy/GFantasy.h"
#include "WorldHUD.generated.h"

class UBossExtraGaugeUserWidget;
/**
 * 
 */
class UPlayerHUD;
class UQuestInstance;
class UBossStateUI;
class AGASCombatBossNPC;
class UWerewolfPattern1UI;

UCLASS()
class GFANTASY_API AWorldHUD : public ABaseHUD
{
	GENERATED_BODY()
	
public:
	AWorldHUD();

public:
	virtual void BeginPlay() override;

	// NPC 상호작용시 위젯 띄우는 함수 / 지우는 건 위젯 내 버튼으로 조작
	void ShowNPCInteractionWidget(FGameplayTag InTag, const FText& InNameText, ENPCType InType);

	UFUNCTION(BlueprintCallable)
	UPlayerHUD* GetPlayerHUDWidget() const { return PlayerHUD; }

	// Quest 목록을 보여주는 함수
	void ShowQuestListWidget(const TArray<UQuestInstance*>& Available, const TArray<UQuestInstance*>& InProgress, const TArray<UQuestInstance*>& Completed);

	void ShowAvailableQuestWidget(const TArray<UQuestInstance*>& Available);
	void ShowInProgressQuestWidget(const TArray<UQuestInstance*>& InProgress);
	void ShowCompletedQuestWidget(const TArray<UQuestInstance*>& Completed);

	// PlayerHUD 에서 WBP_QuestList 준비되면 호출될 함수
	UFUNCTION()
	void HandleQuestListReady(UQuestListWidget* QuestList);

	/* 캐릭터 IMC */
	
	// NPC UI가 켜져 있는지 아닌지를 반환하는 함수,
	UFUNCTION(BlueprintCallable)
	bool IsNPCUIOpen() const;

	
protected:
	// 로그인 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULogInWidget> LogInWidgetClass;

	// NPC 상호작용 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UNPCInteractionWidget> NPCInteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	UPlayerHUD* PlayerHUD;

public:
	// 로그인 위젯 캐싱용
	UPROPERTY()
	class ULogInWidget* LogInWidgetInstance;

	// NPC 상호작용 위젯
	UPROPERTY()
	class UNPCInteractionWidget* NPCInteractionWidget;

private:
	// 퀘스트 위젯
	UPROPERTY()
	class UQuestListWidget* QuestListWidget;
	
	UPROPERTY()
	class UQuestMainWidget* QuestMainWidget;

	/**
	 * 김민형 제작 부분 시작
	 */
public:
	void ShowBossHpBar(AGASCombatBossNPC* BossCharacter);

	void SetBossName(FText NewBossName);

	void SetBossUIHealth(float NewHealth);

	void SetBossUIMaxHealth(float NewMaxHealth);

	void SetBossUIGroggy(float NewGroggy);

	void SetBossUIMaxGroggy(float NewMaxGroggy);
	
	void HideBossHpBar();

	void ShowBossExtraGaugeUI();

	void ExtraGaugeUISpacebarClick();
	
	void SetExtraGaugeUIExtraGauge(float NewExtraGauge);

	void SetExtraGaugeUIMaxExtraGauge(float NewMaxExtraGauge);
	
	void HideBossExtraGaugeUI();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowWerewolfPattern1Progressbar();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void WerewolfPattern1SetProgressbarPercent(float NewPercent);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void HideWerewolfPattern1Progressbar();
	

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBossStateUI> BossStateUIClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBossExtraGaugeUserWidget> BossExtraGaugeUIClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWerewolfPattern1UI> WerewolfPattern1UIClass;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UBossStateUI* BossStateUI;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UBossExtraGaugeUserWidget* BossExtraGaugeUI;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UWerewolfPattern1UI* WerewolfPattern1UI;
	/**
	 * 김민형 제작 부분 끝
	 */
};
