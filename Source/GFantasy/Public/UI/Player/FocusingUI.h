// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FocusingUI.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UFocusingUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	// ���� ��Ŀ�� UI�� ǥ���� Ÿ�� �����ϱ�.
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* Target);
	
protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdatePosition(APlayerController* PC);



private:
	// ���� Ÿ��
	UPROPERTY()
	AActor* TargetActor;
	
	
	
};
