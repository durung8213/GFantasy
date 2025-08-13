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

	// Target�� ���� ���, Homing �ǵ��� �Ѵ�.
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

	// overlap �� �ݸ���
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Collision;
protected:


	// ����ü ������ ���
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;



	// mesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	// ����ü �ִ� ���� �Ÿ�
	UPROPERTY(EditDefaultsOnly, Category = "LifeTime")
	float MaxDistance = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// ���� ��ġ
	FVector SpawendLocation;

};
