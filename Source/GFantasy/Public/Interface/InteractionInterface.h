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
	
	/* ��ȣ�ۿ��� ������ ���͵��� �����ؾ� �� �Լ�
		FŰ�� ������ �Ǹ� �ش� �Լ����� ����ȴ�.
		�ش� ���͵��� Tag�� "Interactable" �� �߰��ؾ� �Ѵ� ( �Ϲ� �±� )*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	/* ��ȣ �ۿ��� ������ ���͵��� UI ǥ�� �Լ�
		�÷��̾�Լ� Ÿ�̸ӷ� ���� ��ȣ�ۿ� ������ ���Ͷ�� 
		�ش� �Լ��� ����Ǿ� UI�� ǥ�õǵ��� �Ѵ�.*/


};


