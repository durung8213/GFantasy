// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/GASWorldPlayerController.h"
#include "KimmhTestWorldPlayerController.generated.h"

class AGASCombatBossNPC;
struct FOnAttributeChangeData;
/**
 * 
 */
UCLASS()
class GFANTASY_API AKimmhTestWorldPlayerController : public AGASWorldPlayerController
{
	GENERATED_BODY()

public:
	// UFUNCTION(Client, Reliable)
	// void ClientShowBossUI(AGASCombatBossNPC* CurrentBoss);
	//
	// void BindBossState(AGASCombatBossNPC* CurrentBoss);
	//
	// void BindBossHealth(const FOnAttributeChangeData& Data);
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void InitBossState(float H ealth, float MaxHealth);
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void SetBossHealth(float NewHealth);
	//
	// UFUNCTION(NetMulticast, Reliable)
	// void SetBossMaxHealth(float NewMaxHealth);
	//
	// void BindBossMaxHealth(const FOnAttributeChangeData& Data);
	//
	// UFUNCTION(Client, Reliable)
	// void ClientDestoryBossUI();
};
