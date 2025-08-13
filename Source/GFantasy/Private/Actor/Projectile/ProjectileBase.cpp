// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Projectile/ProjectileBase.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovement->InitialSpeed = InitialSpeed;
	
	ProjectileMovement->MaxSpeed = MaxSpeed;
	
	ProjectileMovement->bRotationFollowsVelocity = bRotationFollowsVelocity;
	
	ProjectileMovement->bShouldBounce = bBounce;
	
	ProjectileMovement->ProjectileGravityScale = GravityScale;
	
	ProjectileMovement->Velocity = ShotVelocity * ProjectileMovement->InitialSpeed;
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::SetShotterASC(UGASCBase* NewShotterASC)
{
	ShooterASC = NewShotterASC;
}

UGASCBase* AProjectileBase::GetShotterASC()
{
	return ShooterASC;
}
