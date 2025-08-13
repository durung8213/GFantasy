// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DamageEffectApplyToOther.generated.h"

/**
 * 
 */
class UGameplayEffect;

UCLASS()
class GFANTASY_API UAN_DamageEffectApplyToOther : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UGameplayEffect> AttackEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName TraceStartBoneName;

	/**
	 * 트레이스에 더할 값 그자리에 스피어 트레이스를 하고 싶으면 0을 함
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TracePlus;

	/**
	 * 스피어 트레이스의 지름
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TraceRadius = 50.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
};
