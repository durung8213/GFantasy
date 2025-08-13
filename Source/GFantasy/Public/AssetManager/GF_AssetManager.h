// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GF_AssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UGF_AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
	static UGF_AssetManager& Get();

	/* Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
	
	
};
