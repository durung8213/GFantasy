// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestInstance.h"
#include "InteractionQuest.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UInteractionQuest : public UQuestInstance
{
	GENERATED_BODY()
	
public:
	// 상호작용 대상 NPC가 뜨도록
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow) override;

	// 퀘스트 수락시 호출되는 함수
	virtual void OnQuestAccepted_Implementation() override;
	virtual void SetCount(int32 Amount) override;
	
};
