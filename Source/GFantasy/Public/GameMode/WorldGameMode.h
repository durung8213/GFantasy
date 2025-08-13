// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/BaseGameMode.h"
#include "WorldGameMode.generated.h"

class AGASWorldPlayerState;

/**
 * 
 */
UCLASS(Blueprintable)
class GFANTASY_API AWorldGameMode : public ABaseGameMode
{
	GENERATED_BODY()

public:
	AWorldGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION()
	void SetAllPlayerNicknameText();

	// ���� �������� �����ϴ� �Լ�
	UFUNCTION()
	void RespawnPlayerAtVillage(AController* Controller);
};
