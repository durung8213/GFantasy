// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GFantasy/GFantasy.h" // EQuestType ���� ����
#include "GFantasy/Public/GAS/GamePlayTag.h"
#include "GameplayTagContainer.h"         // FGameplayTagContainer

#include "QuestDataAsset.generated.h"

/**
 * ����Ʈ�� ��Ÿ ������ ��� �ִ� DataAsset Ŭ����
 */
UCLASS()
class GFANTASY_API UQuestDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UQuestDataAsset();

	// ���� ����Ʈ ��Ȳ(�Ϸ�, �̿Ϸ�) -> �̰� �Ϸ�Ǿ�� ����Ʈ �±װ� ���ۺҰ��ɿ��� ���۰������� �ٲ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	bool bCanAcceptQuest = true;	// �⺻������ �� ���۰����� ����Ʈ
	/*
	// ���� ����Ʈ �±�(�Ϸ�, �̿Ϸ� -> �̰� �Ϸ�Ǿ�� ����Ʈ �±װ� ���ۺҰ��ɿ��� ���۰������� �ٲ�)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FGameplayTag CanAcceptQuestTag;
	*/


	// ����Ʈ ID (���� �ĺ���)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestID;

	// ����Ʈ �̸�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText QuestName;

	// ����Ʈ ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText QuestDescription;

	// ����Ʈ ���� ��Ȳ�� ��Ÿ���� �±�(Loocked(��������Ʈ �ʿ�), Available(���� ����), InProgress(������), Completed(�Ϸ�))
	// "Quest.State.Available", "Quest.State.InProgress", "Quest.State.Completed"
	// Locked�� bool�� ����ұ� �����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FGameplayTag QuestStateTag;

	// ����Ʈ ��з�(Interaction(��ȭ), Collection(����), KillCount(���), Delivery(����) ��)
	// GFantasy.h�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	EQuestType QuestType;

	// QuestType�� ���� ���� �з�(� ���͸� ������, � �������� ������ ��)
	// "Quest.KillCount.Wolf", "Quest.Collection.HealthPotion" ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FGameplayTagContainer QuestDetailTags;

	////////////////////////////////////////////////////////////
	// �䱸 ���(����, ������) ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName TargetID;

	// �䱸 ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 RequiredCount;

	// ���� �������� ����Ʈ ��Ȳ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 CurrentCount;

	// ����Ʈ ����� (CurrentCount/RequiredCount)
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	int32 QuestProgress;

	///////////////////////////////////////////////////////////////
	// ���� ������ Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName RewardItemID;
	// TSubclassOf<class UItem> RewardItemClass;

	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 RewardCount;
};
