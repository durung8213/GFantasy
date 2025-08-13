// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/CustomizerInterface.h"
#include "PlayerState/GASBasePlayerState.h"
#include "GASWorldPlayerState.generated.h"


class UPlayerCharacterGASC;
class UCustomizerComponent;
/**
 * 
 */
UCLASS()
class GFANTASY_API AGASWorldPlayerState : public AGASBasePlayerState, public ICustomizerInterface
{
	GENERATED_BODY()
	
public:
	AGASWorldPlayerState();

	virtual UCustomizerComponent* GetCustomizerComponent_Implementation() override;

protected:

	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Nickname")
	void SetPlayerNickName(const FString& InName);

	UFUNCTION()
	void OnRep_PlayerNickName();

	UFUNCTION(BlueprintCallable, Category = "Nickname")
	FString GetPlayerNickname() const;


private:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_PlayerNickName, VisibleAnywhere, Category = "Nickname")
	FString PlayerNickname;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Replicated)
	TObjectPtr<UCustomizerComponent> CustomizerComponent;
	
};
