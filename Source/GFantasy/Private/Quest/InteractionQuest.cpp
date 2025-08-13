// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/InteractionQuest.h"

void UInteractionQuest::InitializeFromTable(const FQuestDataTableRow& InRow)
{
	Super::InitializeFromTable(InRow);

	TargetName = ChangeTagToText(QuestEndNPCTag);
}

void UInteractionQuest::OnQuestAccepted_Implementation()
{
	// ĳ�� �� ���� ���� �����ϴ� �θ� �Լ�
	SetCount(1);
	Super::OnQuestAccepted_Implementation();
}

void UInteractionQuest::SetCount(int32 Amount)
{
	Super::SetCount(Amount);

	// InteractionQuest�� �׳� ��ü������ �����ֱ�
	CurrentCount = RequiredCount;

	UpdateProgress();

	// �Ϸᰡ���ϵ��� �ٲٱ�
	bCanComplete = true;
}
