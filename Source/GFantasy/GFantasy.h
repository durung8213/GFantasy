// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EInteractType : uint8
{
	Press UMETA(DisplayName = "Press"),
	Hold UMETA(DisplayName = "Hold"),
	// Max UMETA(DisplayName = "Max"),
};

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	Interaction UMETA(DisplayName = "Interaction"),
	Collection UMETA(DisplayName = "Collection"),
	KillCount UMETA(DisplayName = "KillCount"),
	Delivery UMETA(DisplayName = "Delivery"),
};

// 퀘스트 진행 상태
UENUM(BlueprintType)
enum class EQuestState : uint8
{
	Locked			UMETA(DisplayName = "Locked"),   // 선행 퀘스트 필요
	Available       UMETA(DisplayName = "Available"),
	InProgress      UMETA(DisplayName = "InProgress"),
	Completed       UMETA(DisplayName = "Completed")
};

// NPC 종류 구분
UENUM(BlueprintType)
enum class ENPCType : uint8
{
	Normal      UMETA(DisplayName = "일반 NPC"),
	Quest		UMETA(DisplayName = "퀘스트 NPC"),
	Merchant    UMETA(DisplayName = "상인"),
	Crafter		UMETA(DisplayName = "제작 대장장이"),
	Enhancer	UMETA(DisplayName = "강화 대장장이"),
	Synthesizer UMETA(DisplayName = "합성 대장장이"),
	BlackSmith	UMETA(DisplayName = "대장장이"),
};