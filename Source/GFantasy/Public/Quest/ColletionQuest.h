// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/QuestInstance.h"
#include "ColletionQuest.generated.h"

/**
 * 
 */


UCLASS()
class GFANTASY_API UColletionQuest : public UQuestInstance
{
	GENERATED_BODY()
	
public:
	// TargetName�� �κ��丮���� ���������� �������̵�
	virtual void InitializeFromTable(const FQuestDataTableRow& InRow) override;

	// Quest ������ ȣ��� �Լ�
	virtual void OnQuestAccepted_Implementation() override;

	// CurrentCount������ ���൵ ����
	virtual void SetCount(int32 Amount) override;
	
	// ������ ���� ���� ��������Ʈ �ڵ鷯
	UFUNCTION()
	void HandleItemCountChanged(const FGameplayTag& ItemTag, int32 NewCount);

private:
};
