// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/EnemyAnimInstanceBase.h"
#include "BossEnemyAnimInstanceBase.generated.h"

/**
 * 
 */

class AGASCombatBossNPC;
UCLASS()
class GFANTASY_API UBossEnemyAnimInstanceBase : public UEnemyAnimInstanceBase
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BossEnemyAnimInstanceBase")
	TObjectPtr<AGASCombatBossNPC> CurrentBossEnemyPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BossEnemyAnimInstanceBase")
	bool bGroggy = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BossEnemyAnimInstanceBase")
	int Phase = 0;
};
