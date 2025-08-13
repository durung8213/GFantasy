// Fill out your copyright notice in the Description page of Project Settings.
#include "GameState/GASWorldGameState.h"
#include "GameMode/WorldGameMode.h"


AGASWorldGameState::AGASWorldGameState()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AGASWorldGameState::RequestRespawn(AController* Controller)
{
	if (AWorldGameMode* GM = GetWorld()->GetAuthGameMode<AWorldGameMode>())
	{
		GM->RespawnPlayerAtVillage(Controller);
	}
}
