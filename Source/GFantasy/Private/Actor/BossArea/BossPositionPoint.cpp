// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BossArea/BossPositionPoint.h"

// Sets default values
ABossPositionPoint::ABossPositionPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABossPositionPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABossPositionPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

