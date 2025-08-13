// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/MonsterAreaIndicator/IndicatorTargetActor.h"


AIndicatorTargetActor::AIndicatorTargetActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AIndicatorTargetActor::BeginPlay()
{
	Super::BeginPlay();
}

void AIndicatorTargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGameplayTag AIndicatorTargetActor::GetTargetLocationTag() const
{
	return TargLocationTag;
}

