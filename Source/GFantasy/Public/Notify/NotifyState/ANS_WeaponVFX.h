// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_WeaponVFX.generated.h"

/**
 * 
 */
 
class UNiagaraSystem;
class ABaseWeapon;
 
UCLASS()
class GFANTASY_API UANS_WeaponVFX : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WeaponIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* Niagar;

	UPROPERTY()
	TObjectPtr<ABaseWeapon> SelectWeapon;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	
};
