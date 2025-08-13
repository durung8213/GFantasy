// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CustomizerInterface.generated.h"

class UCustomizerComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCustomizerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GFANTASY_API ICustomizerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCustomizerComponent* GetCustomizerComponent();
	
};
