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

	/*서버 권한으로 캐릭터 설정하기 위한 함수*/
	UCharacterClassInfo* GetCharacterClassDefaultInfo() const;

protected:
	AActor* FindVillagePlayerStart() const;
private:

	/*에디터에서 설정한 데이터 에셋*/
	UPROPERTY(EditDefaultsOnly, Category = "Custom Value|Class Defaults")
	TObjectPtr<UCharacterClassInfo> ClassDefaults;
	
};
