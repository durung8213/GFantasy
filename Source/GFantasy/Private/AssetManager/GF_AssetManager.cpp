// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetManager/GF_AssetManager.h"
#include "GAS/GamePlayTag.h"


UGF_AssetManager& UGF_AssetManager::Get()
{
	UGF_AssetManager* BaseAssetManager = Cast<UGF_AssetManager>(GEngine->AssetManager);
	return *BaseAssetManager;
}

void UGF_AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FGameplayTags::InitializeNativeGameplayTags();
}
