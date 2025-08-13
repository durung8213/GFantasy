// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/AIControllerBase.h"
#include "WolfAIController.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API AWolfAIController : public AAIControllerBase
{
	GENERATED_BODY()
	
public:
	AWolfAIController();

protected:
	virtual void Tick(float DeltaTime) override;
	
	/**
	 * <summary>
	 * 개체를 시야로 감지하였을 때 실행될 함수
	 * </summary>
	 * <param name="SenseActor">감지한 액터</param>
	 */
	virtual void OnEntityDetectedBySense_Implementation(AActor* SenseActor) override;

	/**
	 * <summary>
	 * 개체를 소리로 감지하였을 때 실행될 함수
	 * </summary>
	 * <param name="Actor">감지한 액터</param>
	*/
	virtual void OnEntityDetectedByHearing_Implementation(AActor* Actor) override;

	/**
	 * <summary>
	 * 개체를 데미지를 입었을 때 실행될 함수
	 * </summary>
	 * <param name="Actor">감지한 액터</param>
	 */
	virtual void OnEntityDetectedByDamage_Implementation(AActor* Actor) override;

	
	
};
