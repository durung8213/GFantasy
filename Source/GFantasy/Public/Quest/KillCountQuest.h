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
	// TargetItem�� ���Ϳ��Լ� ������������ �������̵�
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow) override;

	// ����Ʈ ������ ȣ��Ǵ� �Լ�
	virtual void OnQuestAccepted_Implementation() override;
	virtual void SetCount(int32 Amount) override;

	// ���Ͱ� �׾��� �� �� ������ �±׸� ���޹޾� SetCount +1 ���ִ� �Լ�
	UFUNCTION()
	void HandleMonsterKilled(const FGameplayTag& MonsterTag);
};
