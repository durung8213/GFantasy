// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LimitBreakCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class GFANTASY_API ALimitBreakCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALimitBreakCamera();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetTargetActor(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void StartCameraSequence();

	// ī�޶� ����ũ �����ϴ� �Լ� (BP)
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void StartCameraShake();

	// ī�޶� �����̴� ������ �ϴ� Ÿ�Ӷ��� ���� �Լ� (BP)
	UFUNCTION(BlueprintImplementableEvent)
	void StartTimeLine();

	// ī�޶� ��ġ �ʱ�ȭ �Լ� (BP)
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeCameraPosition();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTransform> RelativeAnchors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator CachedRotation;
protected:
	TArray<FTransform> FinalTransforms;
	int32 CurrentIndex;

	UPROPERTY()
	class UTimelineComponent* CameraTimeline;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* Camera;
	
	
};
