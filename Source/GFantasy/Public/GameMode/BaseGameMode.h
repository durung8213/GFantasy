// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseGameMode.generated.h"

class UCharacterClassInfo;
/**
 * 
 */
UCLASS(Blueprintable)
class GFANTASY_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()
	
	
public:

	/*���� �������� ĳ���� �����ϱ� ���� �Լ�*/
	UCharacterClassInfo* GetCharacterClassDefaultInfo() const;

protected:
	AActor* FindVillagePlayerStart() const;
private:

	/*�����Ϳ��� ������ ������ ����*/
	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;
	
};
