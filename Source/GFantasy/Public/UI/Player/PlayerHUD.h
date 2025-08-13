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
	// 위젯이 생성되고 위젯 트리가 완성된 직후에 호출되는 함수
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 캐릭터 관련 상태 UI 초기화 함수
	void InitializeHUD(UAbilitySystemComponent* InASC, UGASCharacterAS* InAS, UGASResourceAS* InResourceAS);

	// 현재 대상에 따라 포커싱 UI를 생성하거나 제거
	UFUNCTION()
	void UpdateTargetUI();

	//// QuestListWidget 준비 시 브로드캐스트
	//UPROPERTY(BlueprintAssignable, Category = "Quest")
	//FOnQuestListReady OnQuestListReady;

public:
	// FocusingUI 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UFocusingUI> FocusingUIClass;
	
	// FocusingWidget으로 바꾸기
	UPROPERTY()
	UFocusingUI* FocusingUIWidget;

	// LockOn UI 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UTargetingUI> TargetingUIClass;

	// LockOn UI Widget
	UPROPERTY()

	
	UTargetingUI* TargetingUIWidget;

public:
	// 화면 왼쪽에 U키를 눌러볼수있는 퀘스트
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UQuestListWidget* WBP_QuestList;

	// 화면 우상단에 상시 출력될 진행중인 퀘스트 목록
	UPROPERTY(meta = (BindWidget))
	class UQuestMainWidget* WBP_QuestMain;

	// 캐릭터 체력 바 UI
	UPROPERTY(meta=(BindWidget))
	class UPlayerHpBar* WBP_PlayerHPBar;

	// 스킬 쿨타임 UI
	// 수정 요망
	// UPROPERTY(meta = (BindWidget))
	// class UCoolDownWidget* WBP_SkillCoolDown;

	// 캐릭터 리미트 게이지 바 UI
	UPROPERTY(meta = (BindWidget))
	class ULimitGaugeUI* WBP_LimitGauge;
	
	UPROPERTY(meta = (BindWidget))
	class USkillSlotWidget* WBP_SkillSlot;
	
private:

	UPROPERTY()
	AActor* CurrentTarget;
	
	bool bIsLockOn = false;
};
