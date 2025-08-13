// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameState/GASBaseGameState.h"
#include "GASWorldGameState.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API AGASWorldGameState : public AGASBaseGameState
{
	GENERATED_BODY()
	
public:

	AGASWorldGameState();

	void RequestRespawn(AController* Controller);
};
