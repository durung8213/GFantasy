// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGame/PlayerSaveData.h"
#include "GFSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGFSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UGFSaveGame();

	// 전체 플레이어 저장 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	FPlayerSaveData PlayerData;
};
