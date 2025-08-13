// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestInstance.h"
#include "KillCountQuest.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UKillCountQuest : public UQuestInstance
{
	GENERATED_BODY()
	
public:
	// TargetItem을 몬스터에게서 가져오기위해 오버라이드
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow) override;

	// 퀘스트 수락시 호출되는 함수
	virtual void OnQuestAccepted_Implementation() override;
	virtual void SetCount(int32 Amount) override;

	// 몬스터가 죽었을 때 그 몬스터의 태그를 전달받아 SetCount +1 해주는 함수
	UFUNCTION()
	void HandleMonsterKilled(const FGameplayTag& MonsterTag);
};
