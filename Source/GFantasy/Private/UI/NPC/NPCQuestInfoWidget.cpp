// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPC/NPCQuestInfoWidget.h"
#include "Quest/QuestManagerComponent.h"
#include "Quest/QuestInstance.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "HUD/WorldHUD.h"
#include "UI/NPC/NPCQuestListWidget.h"
#include "Controller/GASWorldPlayerController.h"
#include "UI/NPC/NPCQuestWidget.h"

void UNPCQuestInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AvailableTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.Available"),  /*bErrorIfNotFound=*/ false);
	InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), /*bErrorIfNotFound=*/ false);

	// 위젯 정보 구성
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UNPCQuestInfoWidget::OnExitButtonClicked);
	}

	if (QuestEnterButton)
	{
		QuestEnterButton->OnClicked.AddDynamic(this, &UNPCQuestInfoWidget::OnQuestEnterButtonClicked);
	}
}

void UNPCQuestInfoWidget::OnExitButtonClicked()
{
	// 위젯 제거
	RemoveFromParent();
	UNPCQuestWidget::SharedInfoWidgetInstance = nullptr;
}

void UNPCQuestInfoWidget::OnQuestEnterButtonClicked()
{
	//if (!Quest || !ManagerComponent) return;
	if (!Quest)
	{
		return;
	}
	if (!ManagerComponent)
	{
		return;
	}

	// 퀘스트 상태에 따라 실행 함수 분리
	if (Quest->QuestStateTag == AvailableTag)
	{
		// 퀘스트 상태가 수락가능일때 수락버튼 클릭
		OnAcceptClicked();
	}
	else if (Quest->QuestStateTag == InProgressTag && Quest->QuestEndNPCTag == InteractNPCTag)
	{
		if (Quest->bCanComplete == true)
		{
			// 퀘스트상태가 진행중이고 완료가능할때 완료 버튼 클릭
			OnCompleteClicked();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("NPCQuestInfoWidget: [InProgress] Con't QuestComplete"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("NPCQuestInfoWidget: No Match Tag"));
	}
}

void UNPCQuestInfoWidget::OnAcceptClicked()
{	
	RemoveFromParent();
	UNPCQuestWidget::SharedInfoWidgetInstance = nullptr;
	ManagerComponent->ServerAcceptQuest(Quest);
}

void UNPCQuestInfoWidget::OnCompleteClicked()
{
	RemoveFromParent();
	UNPCQuestWidget::SharedInfoWidgetInstance = nullptr;
	ManagerComponent->ServerCompleteQuest(Quest);
}

void UNPCQuestInfoWidget::InitializeWithQuest(UQuestInstance* InQuest, UQuestManagerComponent* InManager, UNPCQuestListWidget* InParentList, FGameplayTag InTag)
{
	Quest = InQuest;    // 내부 멤버에 복사
	ManagerComponent = InManager;
	ParentListWidget = InParentList;
	InteractNPCTag = InTag;

	if (!Quest) return;

	if (QuestName)
	{
		QuestName->SetText(Quest->QuestName);
	}

	if (QuestDescription)
	{
		QuestDescription->SetText(Quest->QuestDescription);
	}

	if (TargetName)
	{
		TargetName->SetText(Quest->TargetName);
	}

	if (RequiredCount)
	{
		RequiredCount->SetText(FText::AsNumber(Quest->RequiredCount));
	}

	if (RewardImage)
	{
		RewardImage->SetBrushFromTexture(Quest->RewardItemIcon);
	}

	if (RewardName)
	{
		RewardName->SetText(Quest->RewardItemName);
	}

	if (RewardCount)
	{
		RewardCount->SetText(FText::AsNumber(Quest->RewardCount));
	}

	if (QuestButtonText)
	{
		QuestButtonText->SetText(Quest->ChangeStateTagToText(Quest->QuestStateTag));

		// 상태가 InProgress일 때만 색상 변경
		if (Quest->QuestStateTag.MatchesTagExact(InProgressTag))
		{
			if (Quest->bCanComplete)
			{
				// 완료 가능하면 파란색
				QuestButtonText->SetColorAndOpacity(FSlateColor(CanCompleteColor));
			}
			else
			{
				// 진행 중이지만 완료 불가능하면 빨간색
				QuestButtonText->SetColorAndOpacity(FSlateColor(InProgressColor));
			}
		}
		else
		{
			// 그 외 상태는 기본 흰색으로
			QuestButtonText->SetColorAndOpacity(FSlateColor(NormalColor));
		}
	}
}
