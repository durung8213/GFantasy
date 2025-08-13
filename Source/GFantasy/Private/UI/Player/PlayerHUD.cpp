// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/PlayerHUD.h"
#include "Character/GASPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "UI/Quest/QuestMainWidget.h"
#include "UI/Player/PlayerHpBar.h"
#include "UI/Player/LimitGaugeUI.h"
#include "UI/Player/SkillSlotWidget.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// 에디터에서 BindWidget으로 연결된 QuestWidget이 유효한지 체크
	if (WBP_QuestMain)
	{
		WBP_QuestMain->ClearQuestList();

		//if (AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(GetOwningPlayer()))
		//{
		//	PC->NotifyQuestListReady(WBP_QuestList);
		//}
		//OnQuestListReady.Broadcast(WBP_QuestList);
		UE_LOG(LogTemp, Warning, TEXT("PlayerHUD: Yes Bind QuestWidget"));
	}
	else
	{
		// BindWidget이 실패했다면 로그를 남겨 확인
		UE_LOG(LogTemp, Warning, TEXT("PlayerHUD: No Bind QuestWidget"));
	}
}

void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTargetUI();
}

void UPlayerHUD::InitializeHUD(UAbilitySystemComponent* InASC, UGASCharacterAS* InAS, UGASResourceAS* InResourceAS)
{
	if (WBP_PlayerHPBar)
	{
		// HpBar 초기화.
		WBP_PlayerHPBar->InitializeWithASC(InASC, InAS);
	}

	if (WBP_LimitGauge)
	{
		// LimitGauge 초기화
		WBP_LimitGauge->InitializeWithASC(InASC, InResourceAS);
	}

	if (WBP_SkillSlot)
	{
		WBP_SkillSlot->BindToASC(Cast<UPlayerCharacterGASC>(InASC));
	}
}

void UPlayerHUD::UpdateTargetUI()
{
	// 현재 허드를 가진 플레이어 캐릭터 가져오기
	AGASPlayerCharacter* PC = Cast<AGASPlayerCharacter>(GetOwningPlayerPawn());

	// 클라이언트에서만 작동하도록 구현
	if (!PC || !PC->IsLocallyControlled())
		return;


	// 타겟 정보와 락온 상태
	AActor* NewTarget = PC->GetCurrentTarget();
	bool bNewLockOn = PC->bIsLockOn;

	// 타겟이 갱신되었다면
	if (NewTarget != CurrentTarget || bNewLockOn != bIsLockOn)
	{
		CurrentTarget = NewTarget;
		bIsLockOn = bNewLockOn;


		if (FocusingUIWidget)
		{
			FocusingUIWidget->RemoveFromParent();
			FocusingUIWidget = nullptr;
		}

		if (TargetingUIWidget)
		{
			TargetingUIWidget->RemoveFromParent();
			TargetingUIWidget = nullptr;
		}

		// 타겟 없을 시 UI 삭제
		if (!CurrentTarget)
			return;

		if (bIsLockOn)
		{
			TargetingUIWidget = CreateWidget<UTargetingUI>(GetWorld(), TargetingUIClass);
			if (TargetingUIWidget)
			{
				TargetingUIWidget->SetTarget(CurrentTarget);
				TargetingUIWidget->AddToViewport();
			}
		}
		else
		{
			FocusingUIWidget = CreateWidget<UFocusingUI>(GetWorld(),
				FocusingUIClass);

			if (FocusingUIWidget)
			{
				FocusingUIWidget->SetTarget(CurrentTarget);
				FocusingUIWidget->AddToViewport();
			}
		}
	}
}