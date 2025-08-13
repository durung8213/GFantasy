// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class GFANTASY_API UInteractionInterface : public UInterface
{

	GENERATED_BODY()
};

class GFANTASY_API IInteractionInterface
{
	GENERATED_BODY()

public:
	
	/* 상호작용이 가능한 액터들이 구현해야 할 함수
		F키를 누르게 되면 해당 함수들이 실행된다.
		해당 액터들은 Tag로 "Interactable" 을 추가해야 한다 ( 일반 태그 )*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	/* 상호 작용이 가능한 액터들의 UI 표시 함수
		플레이어에게서 타이머로 현재 상호작용 가능한 액터라면 
		해당 함수가 실행되어 UI가 표시되도록 한다.*/


};


