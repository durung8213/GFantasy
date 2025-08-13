// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

struct FEquipmentActorsToSpawn;
class AWeaponBase;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnWeaponActorSpawned, AWeaponBase*, WeaponActor
);

UCLASS()
class GFANTASY_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()
	
public:

	virtual void OnEquipped();
	virtual void OnUnEquipped();

	
	void SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn);
	void DestroySpawnedActors();

	// 무기 액터가 생성되면 캐릭터에서 호출
	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnWeaponActorSpawned OnWeaponActorSpawned;

	

private:

	UPROPERTY()
	TArray<AActor*> SpawnedActors;

	ACharacter* GetCharacter();
};
