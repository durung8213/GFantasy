// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Title/BaseTitleWidget.h"
#include "TitleSwitcherLogInWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UTitleSwitcherLogInWidget : public UBaseTitleWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	// 로그인 버튼 클릭 시 브로드캐스트되는 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginRequested);

	// 로그인 요청 발생 시 바인딩할 수 있도록 노출
	UPROPERTY(BlueprintAssignable, Category = "TitleLogin")
	FOnLoginRequested OnLoginRequested;

protected:
	// 위젯 전환 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	// 버튼 클릭시 실행될 함수
	UFUNCTION()
	void OnLogInButtonClicked();

	UFUNCTION()
	void HandleLoginButtonClicked();
};
