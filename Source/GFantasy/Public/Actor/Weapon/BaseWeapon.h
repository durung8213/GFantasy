// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class UGameplayEffect;
class UGASCBase;
class UPrimitiveComponent;
class UNiagaraComponent;
UCLASS()
class GFANTASY_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	UFUNCTION()
	void GetOwnerActor();

	UFUNCTION(BlueprintCallable)
	void ClearHitActorsArr();

	UFUNCTION(BlueprintCallable)
	void SetCollisionActive();

	UFUNCTION(BlueprintCallable)
	void SetCollisionInactive();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	virtual void EnemyHit(AActor* OtherActor);

public:
	virtual FTransform GetWeaponSocketWorldTransform(FName Socket);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ActivateTrail();
	void DeactivateTrail();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* OwnerActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UGASCBase* OwnerActorASC;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> AttackEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int AttackEffectIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="VFX")
	UNiagaraComponent* TrailVfx;
	
protected:
	UPROPERTY()
	TArray<AActor*> HitActors;

	UPROPERTY()
	TArray<UPrimitiveComponent*> HitComponents;
	
};
