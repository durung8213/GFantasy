// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Weapon/BaseStaticMeshWeapon.h"
#include "BaseStaticMeshCapsuleWeapon.generated.h"

class UCapsuleComponent;
/**
 * 
 */
UCLASS()
class GFANTASY_API ABaseStaticMeshCapsuleWeapon : public ABaseStaticMeshWeapon
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* HitCollision;
};
