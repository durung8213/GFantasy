// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossEnemyInterface.generated.h"

class AGASPlayerCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBossEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GFANTASY_API IBossEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BossInterface")
	FVector GetBossPositionPointLocationByIndex(int Index);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BossInterface")
	FVector GetBossAreaMiddlePositionPointLocation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "BossInterface")
	TArray<AGASPlayerCharacter*> GetInAreaPlayers();
};
