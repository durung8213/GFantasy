// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/BaseWeapon.h"
#include "BaseStaticMeshWeapon.generated.h"

/**
 * 
 */
class UStaticMeshComponent;

UCLASS()
class GFANTASY_API ABaseStaticMeshWeapon : public ABaseWeapon
{
	GENERATED_BODY()
protected:
	ABaseStaticMeshWeapon();

public:
	virtual FTransform GetWeaponSocketWorldTransform(FName Socket) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WeaponMesh;
};
