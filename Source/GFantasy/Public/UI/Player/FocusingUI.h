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

	// 현재 포커싱 UI를 표시할 타겟 설정하기.
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* Target);
	
protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdatePosition(APlayerController* PC);



private:
	// 현재 타겟
	UPROPERTY()
	AActor* TargetActor;
	
	
	
};
