// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/BaseStaticMeshWeapon.h"
#include "BaseStaticMeshBoxWeapon.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class GFANTASY_API ABaseStaticMeshBoxWeapon : public ABaseStaticMeshWeapon
{
	GENERATED_BODY()

	ABaseStaticMeshBoxWeapon();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* HitCollision;
};
