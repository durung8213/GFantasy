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
	
	// �α��� ��ư Ŭ�� �� ��ε�ĳ��Ʈ�Ǵ� ��������Ʈ
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginRequested);

	// �α��� ��û �߻� �� ���ε��� �� �ֵ��� ����
	UPROPERTY(BlueprintAssignable, Category = "TitleLogin")
	FOnLoginRequested OnLoginRequested;

protected:
	// ���� ��ȯ ��ư
	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	// ��ư Ŭ���� ����� �Լ�
	UFUNCTION()
	void OnLogInButtonClicked();

	UFUNCTION()
	void HandleLoginButtonClicked();
};
