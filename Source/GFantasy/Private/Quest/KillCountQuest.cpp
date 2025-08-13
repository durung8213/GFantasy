// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/KillCountQuest.h"

void UKillCountQuest::InitializeFromTable(const FQuestDataTableRow& InRow)
{
	Super::InitializeFromTable(InRow);

	TargetName = ChangeTagToText(TargetTag);
}

void UKillCountQuest::OnQuestAccepted_Implementation()
{
	Super::OnQuestAccepted_Implementation();

	UE_LOG(LogTemp, Log, TEXT("KillCountQuest : OnQuestAccepted_Implementation"));

}

void UKillCountQuest::SetCount(int32 Amount)
{
	int32 ThisCount = CurrentCount + Amount;
	Super::SetCount(ThisCount);

	UE_LOG(LogTemp, Log, TEXT("KillCountQuest"));
}

void UKillCountQuest::HandleMonsterKilled(const FGameplayTag& MonsterTag)
{
	UE_LOG(LogTemp, Log, TEXT("KillCountQuest : Get MonsterTag"));
	if (TargetTag == MonsterTag)
	{
		SetCount(1);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("KillCountQuest : Not TargetTag"));
	}
}
