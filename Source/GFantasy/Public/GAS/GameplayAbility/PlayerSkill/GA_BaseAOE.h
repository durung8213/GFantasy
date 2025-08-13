// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/GameplayAbility/GA_Base.h"
#include "GA_BaseAOE.generated.h"

class UAbilityTask_SpawnActor;
class UNiagaraSystem;
class APlayerAOEBase;

/**
 * 
 */
UCLASS()
class GFANTASY_API UGA_BaseAOE : public UGA_Base
{
	GENERATED_BODY()
public:
	UGA_BaseAOE();


protected:
	
	// Hit Result로 스폰
	void SpawnAOEActorAtLocation(const FHitResult& Hit);

	// Location 으로 스폰
	void SpawnAOEActorAtLocation(const FVector& Location);

	// 생성한 

	UFUNCTION()
	void OnAOESpawnedSuccess(AActor* SpawnedActor);

	UFUNCTION()
	void OnAOESpawnedFailed(AActor* SpawnedActor);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AOE")
	TSubclassOf<APlayerAOEBase> AOEActorClass;

};
