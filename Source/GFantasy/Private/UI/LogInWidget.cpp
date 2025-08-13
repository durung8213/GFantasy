// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LogInWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "GameFramework/PlayerController.h"             // APlayerController
#include "PlayerState/GASWorldPlayerState.h"   
#include "Controller/GASWorldPlayerController.h"// AGASWorldPlayerState
#include "Blueprint/WidgetBlueprintLibrary.h"            // �Է� ��� ����


void ULogInWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EnterButton)
	{
		EnterButton->OnClicked.AddDynamic(this, &ULogInWidget::OnEnterButtonClicked);
	
		EnterButton->SetIsEnabled(false);
	}

	// 2) �ؽ�Ʈ ���� �̺�Ʈ ���ε�
	if (NicknameInputText)
	{
		NicknameInputText->OnTextChanged.AddDynamic(this, &ULogInWidget::OnNicknameTextChanged);
	}
}

void ULogInWidget::OnEnterButtonClicked()
{
	// ���� ��ư�� ������ �� �Ͼ�� ��
	UE_LOG(LogTemp, Log, TEXT("Nickname : LogInWidget Enter Button Clicked"));

	// NicknameInputTextBlock�� Text��������
	if (!NicknameInputText)
	{
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Nickname : Yes NicknameInputText"));
	}

	const FString EnteredName = NicknameInputText->GetText().ToString();

	UE_LOG(LogTemp, Log, TEXT("Nickname : %s"), *EnteredName);

	// �г��� ������ ������ ������ ĳ���Ϳ��� �����ֱ�
	// ĳ���� ���� �ִ� ������ �ٸ� ĳ������ �г����� ���� �� �ְ�
	if (APlayerController* PC = GetOwningPlayer())
	{
		// ������ ȭ�鿡�� ����
		RemoveFromParent();

		// ���� �÷��̿� �Է� ��� ���Ӹ��� �ٲٱ�
		FInputModeGameOnly GameMode;
		GameMode.SetConsumeCaptureMouseDown(false);
		PC->SetInputMode(GameMode);
		PC->bShowMouseCursor = false;

		AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
		MyPC->ServerSetNickname(EnteredName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No PC"));
	}
}

void ULogInWidget::OnNicknameTextChanged(const FText& Text)
{
	UE_LOG(LogTemp, Log, TEXT("OnNicknameTextChanged: %s"), *Text.ToString());
	// �Էµ� �ؽ�Ʈ�� �˻��Ͽ� EnterButton Ȱ��/��Ȱ�� ����
	const FString Current = Text.ToString();
	const bool bValid = IsValidNickname(Current);

	if (EnterButton)
	{
		EnterButton->SetIsEnabled(bValid);
		UE_LOG(LogTemp, Log, TEXT("Nickname='%s'  IsValid=%s  ButtonEnabled=%s"),
			*Current,
			bValid ? TEXT("true") : TEXT("false"),
			EnterButton->GetIsEnabled() ? TEXT("true") : TEXT("false"));
	}
}

bool ULogInWidget::IsValidNickname(const FString& InName) const
{
	// 1) ���� �˻�: �ּ� 1��, �ִ� 12��
	const int32 Len = InName.Len();
	if (Len == 0 || Len > 12)
	{
		return false;
	}

	// 2) ���� �˻�: ����, ����, �ѱ�(��~�R)�� ���
	for (TCHAR C : InName)
	{
		// ����
		if (C >= '0' && C <= '9')
		{
			continue;
		}
		// ���� �빮��
		if (C >= 'A' && C <= 'Z')
		{
			continue;
		}
		// ���� �ҹ���
		if (C >= 'a' && C <= 'z')
		{
			continue;
		}
		// �ѱ� ����: U+AC00(��) ~ U+D7A3(�R)
		if (C >= 0xAC00 && C <= 0xD7A3)
		{
			continue;
		}

		// �� ���� ���ڰ� ���� ������ ����
		return false;
	}

	return true;  // ��� ���ڰ� ��� ������ ���� true}

}