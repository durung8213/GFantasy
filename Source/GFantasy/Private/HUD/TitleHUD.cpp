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


	// PlayerController 가져오기
	APlayerController* PC = GetOwningPlayerController();
	
	if (!CheckInitWidget(PC))
	{
		UE_LOG(LogTemp, Log, TEXT("ATitleHUD: PlayerController or widget classes not set"));
		return;
	}

	// SaveGame 존재 여부 확인
	const bool bHasSave = UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex);
	TSubclassOf<UTitleWidget> ChosenClass;
	if (!bHasSave)
	{
		// SaveData가 없을 때 : 시작하기
		ChosenClass = TitleWidgetClass;
	}
	else
	{
		// 있을 때 : 이어하기
		ChosenClass = Title2WidgetClass;
	}

	// Title 위젯 생성
	TitleWidget = CreateWidget<UTitleWidget>(PC, ChosenClass);

	if (TitleWidget)
	{
		TitleWidget->AddToViewport();

		// 마우스 커서 보이게 하기
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;

		// 게임 입력과 UI 입력을 함께 받도록 설정
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


	//	// 뷰포트에 추가
	//	TitleWidget->AddToViewport();

	//	// 마우스 커서 보이게 하기
	//	PC->bShowMouseCursor = true;
	//	PC->bEnableClickEvents = true;
	//	PC->bEnableMouseOverEvents = true;

	//	// 게임 입력과 UI 입력을 함께 받도록 설정
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
		// TitleWidgetClass가 없을때
		UE_LOG(LogTemp, Log, TEXT("TitleHUD : No TitleWidgetClass"));
		// 화면에만 출력 (로그는 남기지 않음), 5초간 노출, 노란색 텍스트
		UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : No TitleWidgetClass"), false, true, FLinearColor::Yellow, 5.0f);
		return false;
	}

	if (!PC)
	{
		// PC 못가져왓을때
		UE_LOG(LogTemp, Log, TEXT("TitleHUD : No PC"));
		UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : No PC"), false, true, FLinearColor::Yellow,5.0f);
		return false;
	}

	// Title 위젯 생성
	TitleWidget = CreateWidget<UTitleWidget>(PC, TitleWidgetClass);

	if (!TitleWidget)
	{
		// TitleWidgetInstance가 없을때
		UE_LOG(LogTemp, Log, TEXT("TitleHUD : NO Instance"));
		UKismetSystemLibrary::PrintString(this, TEXT("TitleHUD : NO Instance"), false, true, FLinearColor::Yellow, 5.0f);
		return false;
	}

	return true;
}
