// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/BaseHUD.h"
#include "TitleHUD.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API ATitleHUD : public ABaseHUD
{
	GENERATED_BODY()
	
public:
	// BeginPlay에서 위젯 생성
	virtual void BeginPlay() override;
	
protected:
	// NULL 체크 함수
	virtual bool CheckInitWidget(APlayerController* PC);

	// 블루프린트에서 위젯 할당할 수 있게 EditDefaultsOnly선언
	// 처음할때 사용할 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTitleWidget> TitleWidgetClass;

	// 이어할 때 사용할 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTitleWidget> Title2WidgetClass;

	// 실제 게임에서 보여줄 위젯
	UPROPERTY()
	class UTitleWidget* TitleWidget;

	
	// SaveGame 슬롯 이름, 인덱스
	static const FString SaveSlotName;
	static const int32 SaveUserIndex;
};
