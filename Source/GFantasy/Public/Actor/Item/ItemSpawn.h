// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "ItemSpawn.generated.h"

class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class GFANTASY_API AItemSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawn();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn=true), Replicated)
	bool bRandomSpawn = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn=true), Replicated)
	FVector RandomSpawnVector = FVector(0,0,600.f);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UNiagaraComponent* ItemSpawnNiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true), Replicated)
	FGameplayTag ItemTag;
};
