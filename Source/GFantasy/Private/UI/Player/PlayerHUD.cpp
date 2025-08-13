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

	// �����Ϳ��� BindWidget���� ����� QuestWidget�� ��ȿ���� üũ
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
		// BindWidget�� �����ߴٸ� �α׸� ���� Ȯ��
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
		// HpBar �ʱ�ȭ.
		WBP_PlayerHPBar->InitializeWithASC(InASC, InAS);
	}

	if (WBP_LimitGauge)
	{
		// LimitGauge �ʱ�ȭ
		WBP_LimitGauge->InitializeWithASC(InASC, InResourceAS);
	}

	if (WBP_SkillSlot)
	{
		WBP_SkillSlot->BindToASC(Cast<UPlayerCharacterGASC>(InASC));
	}
}

void UPlayerHUD::UpdateTargetUI()
{
	// ���� ��带 ���� �÷��̾� ĳ���� ��������
	AGASPlayerCharacter* PC = Cast<AGASPlayerCharacter>(GetOwningPlayerPawn());

	// Ŭ���̾�Ʈ������ �۵��ϵ��� ����
	if (!PC || !PC->IsLocallyControlled())
		return;


	// Ÿ�� ������ ���� ����
	AActor* NewTarget = PC->GetCurrentTarget();
	bool bNewLockOn = PC->bIsLockOn;

	// Ÿ���� ���ŵǾ��ٸ�
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

		// Ÿ�� ���� �� UI ����
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