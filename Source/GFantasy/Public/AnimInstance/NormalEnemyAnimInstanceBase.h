// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/EnemyAnimInstanceBase.h"
#include "NormalEnemyAnimInstanceBase.generated.h"

class ANormalEnemyBase;
/**
 * 
 */
UCLASS()
class GFANTASY_API UNormalEnemyAnimInstanceBase : public UEnemyAnimInstanceBase
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="NormalEnemyAnimInstanceBase", meta = (BlueprintThreadSafe))
	int GetRandomIdleIndex();

	void SetRotateValue();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NormalEnemyAnimInstanceBase")
	TObjectPtr<ANormalEnemyBase> CurrentNormalEnemyPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NormalEnemyAnimInstanceBase")
	bool bIsRandomIdle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NormalEnemyAnimInstanceBase")
	bool bRotateR = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NormalEnemyAnimInstanceBase")
	bool bRotateL = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NormalEnemyAnimInstanceBase")
	bool bIsTurningLeftInPlace = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NormalEnemyAnimInstanceBase")
	bool bIsTurningRightInPlace = false;
};
