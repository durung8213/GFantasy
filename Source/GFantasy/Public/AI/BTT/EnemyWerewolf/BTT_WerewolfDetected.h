// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTT/BTT_DetectFocusActorByTick.h"
#include "BTT_WerewolfDetected.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UBTT_WerewolfDetected : public UBTT_DetectFocusActorByTick
{
	GENERATED_BODY()
	
protected:
	virtual void DetectedFocusActor(AAIControllerBase* Owner, AActor* DetectedActor) override;
};
