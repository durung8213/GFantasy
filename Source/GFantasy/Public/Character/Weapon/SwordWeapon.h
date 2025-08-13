// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Weapon/WeaponBase.h"
#include "SwordWeapon.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class GFANTASY_API ASwordWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	ASwordWeapon();

	UFUNCTION(BlueprintCallable)
	void ActivateLimitVFX();

	UFUNCTION(BlueprintCallable)
	void DeactivateLimitVFX();
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UNiagaraComponent* LimitBreakVFX_Sword;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UNiagaraComponent* LimitBreakVFX_Trail;

};
