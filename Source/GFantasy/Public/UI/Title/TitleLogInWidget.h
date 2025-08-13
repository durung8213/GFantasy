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

	// ���� ��ȯ ��ư
	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;
	
	// WidgetSwitcher�� ��ġ�� �α��� �� ����
	UPROPERTY(meta = (Bindwidget))
	class UTitleSwitcherLogInWidget* WBP_TitleSwitcherLogIn;

	//// �α��� ���� �� ���� ���� ���� ���� Ŭ����
	//UPROPERTY(EditAnywhere, Category = "Title")
	//TSubclassOf<class UTitleWidget> TitleWidgetClass;
	
	// ��ư Ŭ���� ����� �Լ�
	UFUNCTION()
	void OnLogInButtonClicked();
	
	UFUNCTION()
	void OnJoinButtonClicked();

private:
	// TitleSwitcherLogInWidget���κ��� ��ε�ĳ��Ʈ�Ǵ� �̺�Ʈ �ڵ鷯
	UFUNCTION()
	void HandleLoginRequested();
};
