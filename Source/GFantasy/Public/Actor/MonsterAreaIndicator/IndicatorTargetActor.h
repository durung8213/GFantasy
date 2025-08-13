// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "IndicatorTargetActor.generated.h"

UCLASS()
class GFANTASY_API AIndicatorTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AIndicatorTargetActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	FGameplayTag GetTargetLocationTag() const;

private:
	// (������ ����) �� NPC �±�
	UPROPERTY(EditAnywhere, Category = "Quest")
	FGameplayTag TargLocationTag;
	
	
};
