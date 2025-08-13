// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstanceBase.generated.h"

/**
 * 
 */

class UCharacterMovementComponent;
class AGASCombatNPC;

UCLASS()
class GFANTASY_API UEnemyAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	TObjectPtr<AGASCombatNPC> CurrentPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	float GroundSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	bool bShouldMove = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	float Direction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="EnemyAnimInstanceBase")
	bool bIsFocus = false;
	
};
