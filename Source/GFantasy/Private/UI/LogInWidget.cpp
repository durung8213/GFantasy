// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LogInWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "GameFramework/PlayerController.h"             // APlayerController
#include "PlayerState/GASWorldPlayerState.h"   
#include "Controller/GASWorldPlayerController.h"// AGASWorldPlayerState
#include "Blueprint/WidgetBlueprintLibrary.h"            // ÀÔ·Â ¸ðµå º¯°æ


void ULogInWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EnterButton)
	{
		EnterButton->OnClicked.AddDynamic(this, &ULogInWidget::OnEnterButtonClicked);
	
		EnterButton->SetIsEnabled(false);
	}

	// 2) ÅØ½ºÆ® º¯°æ ÀÌº¥Æ® ¹ÙÀÎµù
	if (NicknameInputText)
	{
		NicknameInputText->OnTextChanged.AddDynamic(this, &ULogInWidget::OnNicknameTextChanged);
	}
}

void ULogInWidget::OnEnterButtonClicked()
{
	// ¿£ÅÍ ¹öÆ°À» ´­·¶À» ¶§ ÀÏ¾î³ª´Â ÀÏ
	UE_LOG(LogTemp, Log, TEXT("Nickname : LogInWidget Enter Button Clicked"));

	// NicknameInputTextBlockÀÇ Text°¡Á®¿À±â
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

	// ´Ð³×ÀÓ ¼­¹ö·Î º¸³»¼­ °¢°¢ÀÇ Ä³¸¯ÅÍ¿¡°Ô º¸³»ÁÖ±â
	// Ä³¸¯ÅÍ À§¿¡ ÀÖ´Â À§Á¬¿¡ ´Ù¸¥ Ä³¸¯ÅÍÀÇ ´Ð³×ÀÓÀÌ º¸ÀÏ ¼ö ÀÖ°Ô
	if (APlayerController* PC = GetOwningPlayer())
	{
		// À§Á¬À» È­¸é¿¡¼­ Á¦°Å
		RemoveFromParent();

		// °ÔÀÓ ÇÃ·¹ÀÌ¿ë ÀÔ·Â ¸ðµå °ÔÀÓ¸ðµå·Î ¹Ù²Ù±â
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
	// ÀÔ·ÂµÈ ÅØ½ºÆ®¸¦ °Ë»çÇÏ¿© EnterButton È°¼º/ºñÈ°¼º °áÁ¤
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
	// 1) ±æÀÌ °Ë»ç: ÃÖ¼Ò 1ÀÚ, ÃÖ´ë 12ÀÚ
	const int32 Len = InName.Len();
	if (Len == 0 || Len > 12)
	{
		return false;
	}

	// 2) ¹®ÀÚ °Ë»ç: ¿µ¹®, ¼ýÀÚ, ÇÑ±Û(°¡~ÆR)¸¸ Çã¿ë
	for (TCHAR C : InName)
	{
		// ¼ýÀÚ
		if (C >= '0' && C <= '9')
		{
			continue;
		}
		// ¿µ¹® ´ë¹®ÀÚ
		if (C >= 'A' && C <= 'Z')
		{
			continue;
		}
		// ¿µ¹® ¼Ò¹®ÀÚ
		if (C >= 'a' && C <= 'z')
		{
			continue;
		}
		// ÇÑ±Û ¹üÀ§: U+AC00(°¡) ~ U+D7A3(ÆR)
		if (C >= 0xAC00 && C <= 0xD7A3)
		{
			continue;
		}

		// ÀÌ ¿ÜÀÇ ¹®ÀÚ°¡ ¼¯¿© ÀÖÀ¸¸é ½ÇÆÐ
		return false;
	}

	return true;  // ¸ðµç ¹®ÀÚ°¡ Çã¿ë ¹üÀ§ÀÏ ¶§¸¸ true}

}