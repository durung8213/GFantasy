// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "ChestBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(ChestBase, Log, All);

class AItemSpawn;
class UTimelineComponent;

UCLASS()
class GFANTASY_API AChestBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChestBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual void Interact_Implementation(AActor* Interactor) override;
	
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientOpenChest();

protected:
	// 값이 업데이트 될 때 호출되는 함수
	UFUNCTION()
	void ChestOpenTimelineProgress(float Value);

	// 타임라인이 완료될 때 호출될 함수 (선택사항)
	UFUNCTION()
	void ChestOpenTimelineFinished();

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:
	class UWidgetComponent* GetGuideWidgetComponent() const;
protected:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	USceneComponent* ItemSpawnPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	UStaticMeshComponent* ChestBodyMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	UStaticMeshComponent* ChestTopMeshComponent;

	UPROPERTY()
	UTimelineComponent* ChestOpenTimeline; 
	
	// 커브
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	UCurveFloat* ChestOpenCurveFloat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	TSubclassOf<AItemSpawn> SpawnItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true), Replicated, Category = "Chest")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Chest")
	int ItemCountRangeMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Chest")
	int ItemCountRangeMax = 3;

	bool bIsOpened = false;

public:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* AreaSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|UI")
	class UWidgetComponent* GuideWidgetComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction|UI")
	TSubclassOf<class UNPCGuideWidget> DefaultGuideWidgetClass;
	
};
