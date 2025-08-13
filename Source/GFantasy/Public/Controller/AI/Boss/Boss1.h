// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/AI/Boss/BossAIControllerBase.h"
#include "Boss1.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API ABoss1 : public ABossAIControllerBase
{
	GENERATED_BODY()

	ABoss1();
public:
	virtual void BossOnDie_Implementation() override;
};
