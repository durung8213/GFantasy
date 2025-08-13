// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Controller/BaseGASController.h"
#include "GASPlayerController.generated.h"

/**
 * title, lobby, world player controller ÀÇ ºÎ¸ð
 */


class ABaseHUD;
class AGASBasePlayerState;
class AGASBaseGameState;
class ABaseGameMode;
class UBaseGameInstance;

UCLASS()
class GFANTASY_API AGASPlayerController : public ABaseGASController
{
	GENERATED_BODY()

public:
	AGASPlayerController();
	
	UFUNCTION(BlueprintCallable, Category = "BaseController")
	ABaseHUD* GetMyHUD() const;

	UFUNCTION(BlueprintCallable, Category = "BaseController")
	AGASBasePlayerState* GetMyPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "BaseController")
	AGASBaseGameState* GetMyGameState() const;

	UFUNCTION(BlueprintCallable, Category = "BaseController")
	ABaseGameMode* GetMyGameMode() const;

	UFUNCTION(BlueprintCallable, Category = "BaseController")
	UBaseGameInstance* GetMyGameInstance() const;

protected:
	virtual void BeginPlay() override;
	


public:

};
