// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GASPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGASPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UGASPlayerAnimInstance();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void NativeInitializeAnimation() override;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UpperMotion", Replicated)
	bool bHasUpperBodyAction = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Riding", Replicated)
	bool bIsMounted = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Life", Replicated)
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Life", Replicated)
	bool bIsRespawned = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockOn", Replicated)
	bool bIsLockOn = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", Replicated)
	bool bIsInCombat = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", Replicated)
	bool bHasWeapon = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UpperMotion", Replicated)
	bool bUseUpperSlot = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", Replicated)
	bool bIsSprint = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PoseCopy")
	USkeletalMeshComponent* MainMesh;

};
