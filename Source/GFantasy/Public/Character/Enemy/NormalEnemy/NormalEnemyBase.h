// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCombatNPC.h"
#include "NormalEnemyBase.generated.h"
class AEnemySpawner;
class USphereComponent;
/**
 * 
 */
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRandomIdleEnd);

UCLASS()
class GFANTASY_API ANormalEnemyBase : public AGASCombatNPC
{
	GENERATED_BODY()
	
	
protected:
	ANormalEnemyBase();
	
	virtual void BeginPlay() override;
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void AreaCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void AreaCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void OnNormalDamage(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target) override;
	
	virtual void OnStrongDamage(const FGameplayEffectSpec& EffectSpec, const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target) override;
	
	void RandomIdleStart();
	
	void RandomIdleEnd();

	FVector GetCurrentTargetLocation();

	void SetCurrentTargetLocation(FVector NewTargetLocation);

	UFUNCTION(BlueprintCallable, NetMulticast, Category="NormalEnemyBase", Reliable)
	void MultiPlayMontage(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
	virtual void SetControllerFocusActor_Implementation(AActor* NewFocusActor) override;
	
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetControllerFocusActor_Implementation() override;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void MultiWidgetHide();
	
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientWidgetVisible(ESlateVisibility VisibleMode);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientWidgetHide();
	
public:
	FRandomIdleEnd OnRandomIdleEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="NormalEnemyBase")
	TObjectPtr<USphereComponent> AreaCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category ="NormalEnemyBase")
	bool bIsRandomIdle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category ="NormalEnemyBase")
	int RandomIdleIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="NormalEnemyBase")
	int MaxRandomIdleIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NormalEnemyBase")
	FVector SpawnerLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NormalEnemyBase")
	float SpawnerArea = 1000.f;

protected:
	UPROPERTY(Replicated)
	FVector CurrentTargetLocation = FVector::ZeroVector;

protected:
	UPROPERTY(Replicated)
	AActor* CurrentFocusActor = nullptr;
	
public:
	UPROPERTY(Replicated)
	bool bIsTurningLeftInPlace = false;
	
	UPROPERTY(Replicated)
	bool bIsTurningRightInPlace = false;

	UPROPERTY()
	AEnemySpawner* MySpawner;

};
