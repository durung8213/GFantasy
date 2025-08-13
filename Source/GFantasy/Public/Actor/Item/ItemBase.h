// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;

UCLASS()
class GFANTASY_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UBoxComponent* BoxCollision;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UNiagaraComponent* ItemSpawnNiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true), Replicated)
	FGameplayTag ItemTag;
};
