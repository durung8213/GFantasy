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
	// BeginPlay���� ���� ����
	virtual void BeginPlay() override;
	
protected:
	// NULL üũ �Լ�
	virtual bool CheckInitWidget(APlayerController* PC);

	// �������Ʈ���� ���� �Ҵ��� �� �ְ� EditDefaultsOnly����
	// ó���Ҷ� ����� ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTitleWidget> TitleWidgetClass;

	// �̾��� �� ����� ����
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UTitleWidget> Title2WidgetClass;

	// ���� ���ӿ��� ������ ����
	UPROPERTY()
	class UTitleWidget* TitleWidget;

	
	// SaveGame ���� �̸�, �ε���
	static const FString SaveSlotName;
	static const int32 SaveUserIndex;
};
