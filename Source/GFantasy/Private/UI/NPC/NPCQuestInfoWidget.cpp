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

	// ���� ���� ����
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
	// ���� ����
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

	// ����Ʈ ���¿� ���� ���� �Լ� �и�
	if (Quest->QuestStateTag == AvailableTag)
	{
		// ����Ʈ ���°� ���������϶� ������ư Ŭ��
		OnAcceptClicked();
	}
	else if (Quest->QuestStateTag == InProgressTag && Quest->QuestEndNPCTag == InteractNPCTag)
	{
		if (Quest->bCanComplete == true)
		{
			// ����Ʈ���°� �������̰� �Ϸᰡ���Ҷ� �Ϸ� ��ư Ŭ��
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
	Quest = InQuest;    // ���� ����� ����
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

		// ���°� InProgress�� ���� ���� ����
		if (Quest->QuestStateTag.MatchesTagExact(InProgressTag))
		{
			if (Quest->bCanComplete)
			{
				// �Ϸ� �����ϸ� �Ķ���
				QuestButtonText->SetColorAndOpacity(FSlateColor(CanCompleteColor));
			}
			else
			{
				// ���� �������� �Ϸ� �Ұ����ϸ� ������
				QuestButtonText->SetColorAndOpacity(FSlateColor(InProgressColor));
			}
		}
		else
		{
			// �� �� ���´� �⺻ �������
			QuestButtonText->SetColorAndOpacity(FSlateColor(NormalColor));
		}
	}
}
