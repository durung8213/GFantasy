// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_ActiveWeaponByIndex.generated.h"
DECLARE_LOG_CATEGORY_EXTERN(LOG_ANS_ActiveWeaponByIndex, Log, All);
/**
 * 
 */
UCLASS()
class GFANTASY_API UANS_ActiveWeaponByIndex : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int AttackEffectIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int WeaponIndex = 0;
	
	
};
