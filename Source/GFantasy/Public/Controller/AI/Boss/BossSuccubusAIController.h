// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/AI/Boss/BossAIControllerBase.h"
#include "BossSuccubusAIController.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API ABossSuccubusAIController : public ABossAIControllerBase
{
	GENERATED_BODY()
	
public:
	virtual void PlayerInArea(AGASPlayerCharacter* NewPlayer) override;

	virtual void PlayerOutArea(AGASPlayerCharacter* OutPlayer) override;

	virtual void BossOnDie_Implementation() override;

	virtual void OnGroggyMax_Implementation() override;
	
	void StartPhase2();

	void ViewTargetPlayer();
	
protected:
	UPROPERTY()
	FTimerHandle GroggyActiveTimerHandle;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossWerewolfAIController")
	TSubclassOf<UGameplayEffect> JustOneTickHeal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossWerewolfAIController")
	TSubclassOf<UGameplayEffect> Phase2Heal;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BossWerewolfAIController")
	TSubclassOf<UGameplayEffect> Phase2Effect;
};
