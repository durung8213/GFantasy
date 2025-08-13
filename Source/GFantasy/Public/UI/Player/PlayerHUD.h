// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Player/FocusingUI.h"
#include "UI/Player/TargetingUI.h"
#include "PlayerHUD.generated.h"

class UPlayerHpBar;
class UGASCharacterAS;
class UGASResourceAS;
class UAbilitySystemComponent;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestListReady, class UQuestListWidget*, QuestListWidget);

/**
 * 
 */


UCLASS()
class GFANTASY_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// ������ �����ǰ� ���� Ʈ���� �ϼ��� ���Ŀ� ȣ��Ǵ� �Լ�
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ĳ���� ���� ���� UI �ʱ�ȭ �Լ�
	void InitializeHUD(UAbilitySystemComponent* InASC, UGASCharacterAS* InAS, UGASResourceAS* InResourceAS);

	// ���� ��� ���� ��Ŀ�� UI�� �����ϰų� ����
	UFUNCTION()
	void UpdateTargetUI();

	//// QuestListWidget �غ� �� ��ε�ĳ��Ʈ
	//UPROPERTY(BlueprintAssignable, Category = "Quest")
	//FOnQuestListReady OnQuestListReady;

public:
	// FocusingUI Ŭ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UFocusingUI> FocusingUIClass;
	
	// FocusingWidget���� �ٲٱ�
	UPROPERTY()
	UFocusingUI* FocusingUIWidget;

	// LockOn UI Ŭ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UTargetingUI> TargetingUIClass;

	// LockOn UI Widget
	UPROPERTY()

	
	UTargetingUI* TargetingUIWidget;

public:
	// ȭ�� ���ʿ� UŰ�� ���������ִ� ����Ʈ
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UQuestListWidget* WBP_QuestList;

	// ȭ�� ���ܿ� ��� ��µ� �������� ����Ʈ ���
	UPROPERTY(meta = (BindWidget))
	class UQuestMainWidget* WBP_QuestMain;

	// ĳ���� ü�� �� UI
	UPROPERTY(meta=(BindWidget))
	class UPlayerHpBar* WBP_PlayerHPBar;

	// ��ų ��Ÿ�� UI
	// ���� ���
	// UPROPERTY(meta = (BindWidget))
	// class UCoolDownWidget* WBP_SkillCoolDown;

	// ĳ���� ����Ʈ ������ �� UI
	UPROPERTY(meta = (BindWidget))
	class ULimitGaugeUI* WBP_LimitGauge;
	
	UPROPERTY(meta = (BindWidget))
	class USkillSlotWidget* WBP_SkillSlot;
	
private:

	UPROPERTY()
	AActor* CurrentTarget;
	
	bool bIsLockOn = false;
};
