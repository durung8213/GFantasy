// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BaseGameInstance.generated.h"

/**
 * 
 */
//class IOnlineSubsystem;	// 전방선언

UCLASS()
class GFANTASY_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// 세이브로드 함수
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SavePlayerData();

	UFUNCTION(BlueprintCallable, Category = "Save")
	FString GetSavedNickname() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	TArray<FQuestSaveData> GetSavedQuests() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	TArray<FInventorySaveData> GetSavedInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	TArray<FCustomizerSaveData> GetSavedCustomizer() const;

private:
	// 내부에서 공통으로 SaveGame을 로드하여 반환하는 헬퍼
	class UGFSaveGame* LoadSaveGame() const;

	// 슬롯 이름, 인덱스 (필요시 에디터에서 변경 가능)
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	FString SlotName = TEXT("PlayerSaveSlot");

	UPROPERTY(EditDefaultsOnly, Category = "Save")
	uint32 UserIndex = 0;





	/// <summary>
	/// 기본 시작 함수 오버라이드 스팀 OSS를 가져오는 것을 추가함
	/// </summary>
	virtual void Init() override;
//
//	void GetSteamOSS();
//
//	/// <summary>
//	/// ID가져오기
//	/// </summary>
//	/// <returns>스팀 ID FString</returns>
//	UFUNCTION(BlueprintCallable, BlueprintPure)
//	FString GetUserID();
//
//	/// <summary>
//	/// 스팀 닉네임 가져오기
//	/// </summary>
//	/// <returns>닉네임 FString</returns>
//	UFUNCTION(BlueprintCallable, BlueprintPure)
//	FString GetUserNickName();
//
//	/// <summary>
//	/// OSS자체를 가져오기
//	/// </summary>
//	/// <returns>시작할 때 입력받은 OSS</returns>
//	IOnlineSubsystem* GetUserOSS();
//
//public:
//	/// <summary>
//	/// 온라인 서브 시스템을 사용하지 않고 싶을 수도 있으니
//	/// </summary>
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OSS")
//	bool bUseSteamOSS = false;
public:

	// TitleWidget에서 호출해서 서버로 이동하는 함수
	UFUNCTION(BlueprintCallable)
	void TravelToServer(const FString& InputIP);

//protected:
//	IOnlineSubsystem* UserOSS;
};
