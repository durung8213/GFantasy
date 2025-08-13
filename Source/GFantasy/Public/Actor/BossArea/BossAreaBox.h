// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/BossArea/BossAreaBase.h"
#include "BossAreaBox.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class GFANTASY_API ABossAreaBox : public ABossAreaBase
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ABossAreaBox();
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void BossAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	UFUNCTION()
	void BossAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BossArea")
	TObjectPtr<UBoxComponent> BossAreaCollision;
};
