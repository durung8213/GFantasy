// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/AI/Boss/BossAIControllerBase.h"
#include "BossWereWolf.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API ABossWereWolf : public ABossAIControllerBase
{
	GENERATED_BODY()

public:
	virtual void PlayerInArea(AGASPlayerCharacter* NewPlayer) override;

	virtual void PlayerOutArea(AGASPlayerCharacter* OutPlayer) override;

	virtual void BossOnDie_Implementation() override;

	void StartPhase2();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossWerewolfAIController")
	TSubclassOf<UGameplayEffect> Phase2Effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossWerewolfAIController")
	TSubclassOf<UGameplayEffect> Phase2Heal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossWerewolfAIController")
	TSubclassOf<UGameplayEffect> JustOneTickHeal;
};
