// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Title/BaseTitleWidget.h"
#include "TitleLogInWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UTitleLogInWidget : public UBaseTitleWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* LogInSwitcher;

	// 위젯 전환 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;
	
	// WidgetSwitcher에 배치된 로그인 탭 위젯
	UPROPERTY(meta = (Bindwidget))
	class UTitleSwitcherLogInWidget* WBP_TitleSwitcherLogIn;

	//// 로그인 성공 후 열고 싶은 다음 위젯 클래스
	//UPROPERTY(EditAnywhere, Category = "Title")
	//TSubclassOf<class UTitleWidget> TitleWidgetClass;
	
	// 버튼 클릭시 실행될 함수
	UFUNCTION()
	void OnLogInButtonClicked();
	
	UFUNCTION()
	void OnJoinButtonClicked();

private:
	// TitleSwitcherLogInWidget으로부터 브로드캐스트되는 이벤트 핸들러
	UFUNCTION()
	void HandleLoginRequested();
};
