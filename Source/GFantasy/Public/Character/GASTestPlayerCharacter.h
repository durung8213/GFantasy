// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/GASPlayerCharacter.h"
#include "Customization/CustomizerComponent.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "Interface/CustomizerInterface.h"
#include "GASTestPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API AGASTestPlayerCharacter : public AGASPlayerCharacter
{
	GENERATED_BODY()
	
	
/*
public:
	AGASTestPlayerCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	void SetCustomizeMesh(const FGFCustomizerEntry& CustomizerEntry);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Customize")
	USkeletalMeshComponent* GetMeshPart(FName CategoryName) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FName,	USkeletalMeshComponent*> MeshParts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FName, UMaterialInstanceDynamic*> PartsMIDs;

protected:

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

private:

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCustomizationMesh(const FGFCustomizerEntry& CustomizerEntry);
	
	/*델리게이트 바인드를 하는 함수#1#
	void BindCallbacksToDependencies();*/
};
