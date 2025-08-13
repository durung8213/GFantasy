// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LogInWidget.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API ULogInWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnEnterButtonClicked();

	// �г��� �ؽ�Ʈ ���� �� ȣ��
	UFUNCTION()
	void OnNicknameTextChanged(const FText& Text);

private:
	bool IsValidNickname(const FString& InName) const;

public:
	UPROPERTY(meta = (BindWidget))
	class UEditableText* NicknameInputText;

	UPROPERTY(meta = (BindWidget))
	class UButton* EnterButton;
	
};
