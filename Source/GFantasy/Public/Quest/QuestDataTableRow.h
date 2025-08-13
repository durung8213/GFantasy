// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"     // FTableRowBase ���� ����
#include "GameplayTagContainer.h"       // FGameplayTag, FGameplayTagContainer
#include "GFantasy/GFantasy.h"          // EQuestType
#include "QuestDataTableRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct GFANTASY_API FQuestDataTableRow : public FTableRowBase
{
	GENERATED_BODY()


public:
	FQuestDataTableRow();

public:
	// ���� ����Ʈ ��Ȳ(�Ϸ�, �̿Ϸ�) -> �̰� �Ϸ�Ǿ�� ����Ʈ �±װ� ���ۺҰ��ɿ��� ���۰������� �ٲ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	bool bCanAcceptQuest = true;	// �⺻������ �� ���۰����� ����Ʈ

	// ����Ʈ ID (���� �ĺ���)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName D_QuestID;

	// ����Ʈ �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText D_QuestName;

	// ����Ʈ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText D_QuestDescription;


	// ����Ʈ�� �����ϴ� NPC �±�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_QuestStartNPCTag;

	// ����Ʈ�� �Ϸ��� �� �ִ� NPC �±�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_QuestEndNPCTag;

	// ����Ʈ ��з�(Interaction(��ȭ), Collection(����), KillCount(���), Delivery(����) ��)
	// GFantasy.h�� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EQuestType D_QuestType;

	// ����Ʈ ���� �±�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_QuestStateTag;


	////////////////////////////////////////////////////////////
	// �䱸 ���(����, ������) ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_TargetTag;

	// �䱸 ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 D_RequiredCount;

	// ���� �������� ����Ʈ ��Ȳ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 D_CurrentCount;


	// �ε������Ͱ� ����ų �±�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_TargetLocationTag;


	///////////////////////////////////////////////////////////////
	// ���� ������ Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_RewardItemTag;

	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 D_RewardCount;


	///////////////////////////////////////////////////////////////
	// ��������Ʈ�� ���� ���� ����Ʈ ���̵�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName D_NextQuestID;
};
