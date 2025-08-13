// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UProjectileMovementComponent;
class UGASCBase;

UCLASS()
class GFANTASY_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetShotterASC(UGASCBase* NewShotterASC);

	UGASCBase* GetShotterASC();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileBase")
	UProjectileMovementComponent* ProjectileMovement;

	// 발사 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true) ,Category = "ProjectileBase")
	FVector ShotVelocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true) ,Category = "ProjectileBase")
	float InitialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true) ,Category = "ProjectileBase")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true) ,Category = "ProjectileBase")
	bool bRotationFollowsVelocity = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ProjectileBase")
	bool bBounce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ProjectileBase")
	int BounceCount = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ProjectileBase")
	float GravityScale = 0.f;
	
	// 이 프로젝타일을 쏜 액터의 ASC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "ProjectileBase")
	TObjectPtr<UGASCBase> ShooterASC;
	
};
