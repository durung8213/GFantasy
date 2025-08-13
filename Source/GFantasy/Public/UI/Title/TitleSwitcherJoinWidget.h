// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Title/BaseTitleWidget.h"
#include "TitleSwitcherJoinWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UTitleSwitcherJoinWidget : public UBaseTitleWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	// 위젯 전환 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* CheckButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;


	// 버튼 클릭시 실행될 함수
	UFUNCTION()
	void OnCheckButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();
	
	
};
