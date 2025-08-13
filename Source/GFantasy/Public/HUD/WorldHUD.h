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

	// NPC ��ȣ�ۿ�� ���� ���� �Լ� / ����� �� ���� �� ��ư���� ����
	void ShowNPCInteractionWidget(FGameplayTag InTag, const FText& InNameText, ENPCType InType);

	UFUNCTION(BlueprintCallable)
	UPlayerHUD* GetPlayerHUDWidget() const { return PlayerHUD; }

	// Quest ����� �����ִ� �Լ�
	void ShowQuestListWidget(const TArray<UQuestInstance*>& Available, const TArray<UQuestInstance*>& InProgress, const TArray<UQuestInstance*>& Completed);

	void ShowAvailableQuestWidget(const TArray<UQuestInstance*>& Available);
	void ShowInProgressQuestWidget(const TArray<UQuestInstance*>& InProgress);
	void ShowCompletedQuestWidget(const TArray<UQuestInstance*>& Completed);

	// PlayerHUD ���� WBP_QuestList �غ�Ǹ� ȣ��� �Լ�
	UFUNCTION()
	void HandleQuestListReady(UQuestListWidget* QuestList);

	/* ĳ���� IMC */
	
	// NPC UI�� ���� �ִ��� �ƴ����� ��ȯ�ϴ� �Լ�,
	UFUNCTION(BlueprintCallable)
	bool IsNPCUIOpen() const;

	
protected:
	// �α��� ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULogInWidget> LogInWidgetClass;

	// NPC ��ȣ�ۿ� ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UNPCInteractionWidget> NPCInteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	UPlayerHUD* PlayerHUD;

public:
	// �α��� ���� ĳ�̿�
	UPROPERTY()
	class ULogInWidget* LogInWidgetInstance;

	// NPC ��ȣ�ۿ� ����
	UPROPERTY()
	class UNPCInteractionWidget* NPCInteractionWidget;

private:
	// ����Ʈ ����
	UPROPERTY()
	class UQuestListWidget* QuestListWidget;
	
	UPROPERTY()
	class UQuestMainWidget* QuestMainWidget;

	/**
	 * ����� ���� �κ� ����
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
	 * ����� ���� �κ� ��
	 */
};
