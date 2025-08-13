// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_RotateCurrentTarget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UANS_RotateCurrentTarget : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual void NotifyBegin(
	USkeletalMeshComponent* MeshComp, 
	UAnimSequenceBase* Animation, 
	float TotalDuration, 
	const FAnimNotifyEventReference& EventReference) override;

	
	
	
};
