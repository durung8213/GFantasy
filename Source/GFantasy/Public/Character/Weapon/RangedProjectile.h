// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RangedProjectile.generated.h"

class USphereComponent;
class UGameplayEffect;

UCLASS()
class GFANTASY_API ARangedProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARangedProjectile();

	// Target이 있을 경우, Homing 되도록 한다.
	void SetHomingTarget(AActor* Target);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void PostInitializeComponents() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// overlap 할 콜리전
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision;
protected:


	// 투사체 움직임 요소
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;



	// mesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	// 투사체 최대 날라갈 거리
	UPROPERTY(EditDefaultsOnly, Category = "LifeTime")
	float MaxDistance = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// 스폰 위치
	FVector SpawendLocation;

};
