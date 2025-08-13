// Fill out your copyright notice in the Description page of Project Settings.

#include "Controller/GASPlayerController.h"
#include "HUD/BaseHUD.h"
#include "GameState/GASBaseGameState.h"
#include "PlayerState/GASBasePlayerState.h"
#include "GameMode/BaseGameMode.h"
#include "GameInstance/BaseGameInstance.h"


AGASPlayerController::AGASPlayerController()
{
}

ABaseHUD* AGASPlayerController::GetMyHUD() const
{
	return Cast<ABaseHUD>(GetHUD());
}

AGASBasePlayerState* AGASPlayerController::GetMyPlayerState() const
{
	return Cast<AGASBasePlayerState>(PlayerState);
}

AGASBaseGameState* AGASPlayerController::GetMyGameState() const
{
	if (UWorld* W = GetWorld())
	{
		return Cast<AGASBaseGameState>(W->GetGameState());
	}
	return nullptr;
}

ABaseGameMode* AGASPlayerController::GetMyGameMode() const
{
	if (UWorld* W = GetWorld())
	{
		return Cast<ABaseGameMode>(W->GetAuthGameMode());
	}
	return nullptr;
}

UBaseGameInstance* AGASPlayerController::GetMyGameInstance() const
{
	if (UWorld* W = GetWorld())
	{
		return Cast<UBaseGameInstance>(W->GetGameInstance());
	}
	return nullptr;
}

void AGASPlayerController::BeginPlay()
{
	Super::BeginPlay();

}
