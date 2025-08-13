// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/BaseWeapon.h"
#include "BaseSkeletalMeshWeapon.generated.h"

/**
 * 
 */
class USkeletalMeshComponent;

UCLASS()
class GFANTASY_API ABaseSkeletalMeshWeapon : public ABaseWeapon
{
	GENERATED_BODY()
protected:
	ABaseSkeletalMeshWeapon();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;
};
