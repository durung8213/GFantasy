// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/TitleHUD.h"
#include "UI/Title/TitleWidget.h"
#include "UI/Title/Title2Widget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Title/TitleLogInWidget.h"

const FString ATitleHUD::SaveSlotName = TEXT("PlayerSaveSlot");
const int32   ATitleHUD::SaveUserIndex = 0;

void ATitleHUD::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("TitleHUD : BeginPlay"));
	UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : BeginPlay"), false, true, FLinearColor::Yellow, 5.0f);


	// PlayerController ��������
	APlayerController* PC = GetOwningPlayerController();
	
	if (!CheckInitWidget(PC))
	{
		UE_LOG(LogTemp, Log, TEXT("ATitleHUD: PlayerController or widget classes not set"));
		return;
	}

	// SaveGame ���� ���� Ȯ��
	const bool bHasSave = UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex);
	TSubclassOf<UTitleWidget> ChosenClass;
	if (!bHasSave)
	{
		// SaveData�� ���� �� : �����ϱ�
		ChosenClass = TitleWidgetClass;
	}
	else
	{
		// ���� �� : �̾��ϱ�
		ChosenClass = Title2WidgetClass;
	}

	// Title ���� ����
	TitleWidget = CreateWidget<UTitleWidget>(PC, ChosenClass);

	if (TitleWidget)
	{
		TitleWidget->AddToViewport();

		// ���콺 Ŀ�� ���̰� �ϱ�
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		// ���� �Է°� UI �Է��� �Բ� �޵��� ����
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TitleWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
	}

	//if (CheckInitWidget(PC))
	//{
	//	UE_LOG(LogTemp, Log, TEXT("TitleHUD : Check Yes"));
	//	UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : Check Yes"), false, true, FLinearColor::Yellow, 5.0f);


	//	// ����Ʈ�� �߰�
	//	TitleWidget->AddToViewport();

	//	// ���콺 Ŀ�� ���̰� �ϱ�
	//	PC->bShowMouseCursor = true;
	//	PC->bEnableClickEvents = true;
	//	PC->bEnableMouseOverEvents = true;

	//	// ���� �Է°� UI �Է��� �Բ� �޵��� ����
	//	FInputModeGameAndUI InputMode;
	//	InputMode.SetWidgetToFocus(TitleWidget->TakeWidget());
	//	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	//	InputMode.SetHideCursorDuringCapture(false);
	//	PC->SetInputMode(InputMode);
	//}
}

bool ATitleHUD::CheckInitWidget(APlayerController* PC)
{
	if (!TitleWidgetClass)
	{
		// TitleWidgetClass�� ������
		UE_LOG(LogTemp, Log, TEXT("TitleHUD : No TitleWidgetClass"));
		// ȭ�鿡�� ��� (�α״� ������ ����), 5�ʰ� ����, ����� �ؽ�Ʈ
		UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : No TitleWidgetClass"), false, true, FLinearColor::Yellow, 5.0f);
		return false;
	}

	if (!PC)
	{
		// PC ������������
		UE_LOG(LogTemp, Log, TEXT("TitleHUD : No PC"));
		UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : No PC"), false, true, FLinearColor::Yellow,5.0f);
		return false;
	}

	// Title ���� ����
	TitleWidget = CreateWidget<UTitleWidget>(PC, TitleWidgetClass);

	if (!TitleWidget)
	{
		// TitleWidgetInstance�� ������
		UE_LOG(LogTemp, Log, TEXT("TitleHUD : NO Instance"));
		UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : NO Instance"), false, true, FLinearColor::Yellow, 5.0f);
		return false;
	}

	return true;
}
