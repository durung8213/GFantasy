// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/InteractionQuest.h"

void UInteractionQuest::InitializeFromTable(const FQuestDataTableRow& InRow)
{
	Super::InitializeFromTable(InRow);

	TargetName = ChangeTagToText(QuestEndNPCTag);
}

void UInteractionQuest::OnQuestAccepted_Implementation()
{
	// 캐시 후 수락 상태 변경하는 부모 함수
	SetCount(1);
	Super::OnQuestAccepted_Implementation();
}

void UInteractionQuest::SetCount(int32 Amount)
{
	Super::SetCount(Amount);

	// InteractionQuest는 그냥 자체적으로 맞춰주기
	CurrentCount = RequiredCount;

	UpdateProgress();

	// 완료가능하도록 바꾸기
	bCanComplete = true;
}
