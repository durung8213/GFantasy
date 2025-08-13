// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Spawner/EnemySpawner.h"
#include "BossEnemySpawner.generated.h"

/**
 * 
 */

class ABossAreaBase;

UCLASS()
class GFANTASY_API ABossEnemySpawner : public AEnemySpawner
{
	GENERATED_BODY()

public:
	virtual void Spawn() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossEnemySpawner")
	ABossAreaBase* BossArea;
};
