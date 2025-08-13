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
	// ��ȣ�ۿ� ��� NPC�� �ߵ���
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow) override;

	// ����Ʈ ������ ȣ��Ǵ� �Լ�
	virtual void OnQuestAccepted_Implementation() override;
	virtual void SetCount(int32 Amount) override;
	
};
