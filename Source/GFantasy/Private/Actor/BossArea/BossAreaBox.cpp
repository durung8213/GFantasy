// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/BossArea/BossAreaBox.h"

#include "Character/GASPlayerCharacter.h"
#include "Components/BoxComponent.h"

ABossAreaBox::ABossAreaBox()
{
	BossAreaCollision = CreateDefaultSubobject<UBoxComponent>("BossAreaCollision");
	SetRootComponent(BossAreaCollision);
	BossAreaCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
}

void ABossAreaBox::BeginPlay()
{
	Super::BeginPlay();
	BossAreaCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossAreaBox::BossAreaBeginOverlap);
	BossAreaCollision->OnComponentEndOverlap.AddDynamic(this, &ABossAreaBox::BossAreaEndOverlap);
}

void ABossAreaBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossAreaBox::BossAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AGASPlayerCharacter* CurrentOtherActor = Cast<AGASPlayerCharacter>(OtherActor))
	{
		OnPlayerBeginOverlap(CurrentOtherActor);
	}
	
}

void ABossAreaBox::BossAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AGASPlayerCharacter* CurrentOtherActor = Cast<AGASPlayerCharacter>(OtherActor))
	{
		OnPlayerEndOverlap(CurrentOtherActor);
	}
	
}
