// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"     // FTableRowBase 정의 포함
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
	// 선행 퀘스트 상황(완료, 미완료) -> 이게 완료되어야 퀘스트 태그가 시작불가능에서 시작가능으로 바뀜
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	bool bCanAcceptQuest = true;	// 기본적으로 다 시작가능한 퀘스트

	// 퀘스트 ID (고유 식별자)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName D_QuestID;

	// 퀘스트 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText D_QuestName;

	// 퀘스트 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText D_QuestDescription;


	// 퀘스트를 제공하는 NPC 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_QuestStartNPCTag;

	// 퀘스트를 완료할 수 있는 NPC 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_QuestEndNPCTag;

	// 퀘스트 대분류(Interaction(대화), Collection(수집), KillCount(사냥), Delivery(전달) 등)
	// GFantasy.h에 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	EQuestType D_QuestType;

	// 퀘스트 상태 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_QuestStateTag;


	////////////////////////////////////////////////////////////
	// 요구 대상(몬스터, 아이템) ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_TargetTag;

	// 요구 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 D_RequiredCount;

	// 현재 진행중인 퀘스트 상황
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 D_CurrentCount;


	// 인디케이터가 가리킬 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_TargetLocationTag;


	///////////////////////////////////////////////////////////////
	// 보상 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag D_RewardItemTag;

	// 보상 수량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 D_RewardCount;


	///////////////////////////////////////////////////////////////
	// 연계퀘스트를 위한 다음 퀘스트 아이디
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName D_NextQuestID;
};
