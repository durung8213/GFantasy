// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPC/NPCQuestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HUD/WorldHUD.h"
#include "Quest/QuestInstance.h"
#include "Quest/QuestManagerComponent.h"
#include "GameFramework/PlayerController.h"
#include "UI/NPC/NPCQuestInfoWidget.h"

UNPCQuestInfoWidget* UNPCQuestWidget::SharedInfoWidgetInstance = nullptr;

void UNPCQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuestButton)
	{
		QuestButton->OnClicked.AddDynamic(this, &UNPCQuestWidget::OnQuestButtonClicked);
	}
}

void UNPCQuestWidget::InitializeWithQuest(UQuestInstance* InQuest, UQuestManagerComponent* InManager, UNPCQuestListWidget* InListWidget, FGameplayTag InTag)
{
    Quest = InQuest;    // 내부 멤버에 복사
    Manager = InManager;
    ParentListWidget = InListWidget;
    InteractNPCTag = InTag;

    // 1) 퀘스트 이름 설정
    if (QuestName && Quest)
    {
        QuestName->SetText(Quest->QuestName);
    }

    // 2) 보상 아이템 이름 설정
    if (RewardItemName && Quest)
    {
        // 태그를 이용해서 아이템 이름 가져오기
        RewardItemName->SetText(Quest->RewardItemName);
    }

    // 3) 퀘스트 상태 태그 문자열로 표시
    if (QuestState && Quest)
    {
        QuestState->SetText(Quest->ChangeStateTagToText(Quest->QuestStateTag));

        // InProgress 태그 준비
        const FGameplayTag InProgressTag = FGameplayTag::RequestGameplayTag(TEXT("Quest.State.InProgress"), false);

        // 상태가 InProgress일 때만 색상 변경
        if (Quest->QuestStateTag.MatchesTagExact(InProgressTag))
        {
            if (Quest->bCanComplete)
            {
                // 완료 가능하면 파란색
                QuestState->SetColorAndOpacity(FSlateColor(CanCompleteColor));
            }
            else
            {
                // 진행 중이지만 완료 불가능하면 빨간색
                QuestState->SetColorAndOpacity(FSlateColor(InProgressColor));
            }
        }
        else
        {
            // 그 외 상태는 기본 흰색으로
            QuestState->SetColorAndOpacity(FSlateColor(NormalColor));
        }
    }
}

void UNPCQuestWidget::CloseNPCQuestWidget()
{
    RemoveFromParent();
}

void UNPCQuestWidget::OnQuestButtonClicked()
{
    // Quest가 유효하지 않으면 무시
    if (!Quest)
    {
        return;
    }

    // 플레이어 컨트롤러 가져오기
    if (APlayerController* PC = GetOwningPlayer())
    {
        // 정보 위젯을 최초 한 번만 생성
        if (!SharedInfoWidgetInstance)
        {
            SharedInfoWidgetInstance = CreateWidget<UNPCQuestInfoWidget>(PC, NPCQuestInfoWidgetClass);
            SharedInfoWidgetInstance->AddToViewport();
        }

        // 이후엔 데이터만 갱신
        SharedInfoWidgetInstance->InitializeWithQuest(Quest, Manager, ParentListWidget, InteractNPCTag);

        // 화면에 없다면 다시 추가
        if (!SharedInfoWidgetInstance->IsInViewport())
        {
            SharedInfoWidgetInstance->AddToViewport();
        }
    }
}
