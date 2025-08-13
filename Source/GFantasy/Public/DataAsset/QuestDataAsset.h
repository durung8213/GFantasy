// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GFantasy/GFantasy.h" // EQuestType 정의 포함
#include "GFantasy/Public/GAS/GamePlayTag.h"
#include "GameplayTagContainer.h"         // FGameplayTagContainer

#include "QuestDataAsset.generated.h"

/**
 * 퀘스트의 메타 정보를 담고 있는 DataAsset 클래스
 */
UCLASS()
class GFANTASY_API UQuestDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UQuestDataAsset();

	// 선행 퀘스트 상황(완료, 미완료) -> 이게 완료되어야 퀘스트 태그가 시작불가능에서 시작가능으로 바뀜
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	bool bCanAcceptQuest = true;	// 기본적으로 다 시작가능한 퀘스트
	/*
	// 선행 퀘스트 태그(완료, 미완료 -> 이게 완료되어야 퀘스트 태그가 시작불가능에서 시작가능으로 바뀜)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FGameplayTag CanAcceptQuestTag;
	*/


	// 퀘스트 ID (고유 식별자)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestID;

	// 퀘스트 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText QuestName;

	// 퀘스트 설명
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText QuestDescription;

	// 퀘스트 진행 상황을 나타내는 태그(Loocked(선행퀘스트 필요), Available(시작 가능), InProgress(진행중), Completed(완료))
	// "Quest.State.Available", "Quest.State.InProgress", "Quest.State.Completed"
	// Locked는 bool을 사용할까 고려중
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FGameplayTag QuestStateTag;

	// 퀘스트 대분류(Interaction(대화), Collection(수집), KillCount(사냥), Delivery(전달) 등)
	// GFantasy.h에 정의
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	EQuestType QuestType;

	// QuestType에 따른 세부 분류(어떤 몬스터를 잡을지, 어떤 아이템을 수집할 지)
	// "Quest.KillCount.Wolf", "Quest.Collection.HealthPotion" 등
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FGameplayTagContainer QuestDetailTags;

	////////////////////////////////////////////////////////////
	// 요구 대상(몬스터, 아이템) ID
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName TargetID;

	// 요구 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 RequiredCount;

	// 현재 진행중인 퀘스트 상황
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 CurrentCount;

	// 퀘스트 진행률 (CurrentCount/RequiredCount)
	UPROPERTY(BlueprintReadOnly, Category = "Quest")
	int32 QuestProgress;

	///////////////////////////////////////////////////////////////
	// 보상 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName RewardItemID;
	// TSubclassOf<class UItem> RewardItemClass;

	// 보상 수량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 RewardCount;
};
