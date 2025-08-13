// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GASCombatNPC.h"
#include "Interface/BossEnemyInterface.h"
#include "GASCombatBossNPC.generated.h"

/**
 * 
 */

class UGASBossGroggy;
class ABossAreaBase;

UCLASS()
class GFANTASY_API AGASCombatBossNPC : public AGASCombatNPC, public IBossEnemyInterface
{
	GENERATED_BODY()

public:
	AGASCombatBossNPC();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override ;
protected:

	
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BossInterface")
	FVector GetBossPositionPointLocationByIndex(int Index);
	virtual FVector GetBossPositionPointLocationByIndex_Implementation(int Index) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BossInterface")
	FVector GetBossAreaMiddlePositionPointLocation();
	virtual FVector GetBossAreaMiddlePositionPointLocation_Implementation() override;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BossInterface")
	TArray<AGASPlayerCharacter*> GetInAreaPlayers();
	virtual TArray<AGASPlayerCharacter*> GetInAreaPlayers_Implementation() override;


	/// <summary>
	/// CombatAttributeSet을 반환하는 함수
	/// </summary>
	/// <returns>AttributeSet</returns>
	virtual UGASBossGroggy* GetGroggyAttributeSet() const;
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	UGASBossGroggy* GroggyAttributeSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	ABossAreaBase* BossArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss", Replicated)
	bool bIsGroggy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatNPC")
	TArray<FGameplayTag> PassivePickupItemArr;
};
