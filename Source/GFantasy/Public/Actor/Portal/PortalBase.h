// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalBase.generated.h"

class AGASPlayerCharacter;

UCLASS()
class GFANTASY_API APortalBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable)
	void AddPlayer(AGASPlayerCharacter* Player);

	void RemovePlayer(AGASPlayerCharacter* Player);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AGASPlayerCharacter*> TpUsePlayerArr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PortalCoolTime = 8.f;
};
