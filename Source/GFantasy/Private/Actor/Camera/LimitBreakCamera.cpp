// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Camera/LimitBreakCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ALimitBreakCamera::ALimitBreakCamera()
{
 	PrimaryActorTick.bCanEverTick = true;
	CameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraTimeline"));

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
}

void ALimitBreakCamera::StartCameraSequence()
{
	// if (!TargetActor || RelativeAnchors.IsEmpty())
	// 	return;
	//
	// FinalTransforms.Empty();
	// for (const FTransform& Rel : RelativeAnchors)
	// {
	// 	FinalTransforms.Add(Rel * TargetActor->GetActorTransform());
	// }
	//
	// CurrentIndex = 0;
	//
	// //위치 초기화
	// SetActorTransform(FinalTransforms[0]);

	// 타임라인 실행
	StartTimeLine();
}

// Called when the game starts or when spawned
void ALimitBreakCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALimitBreakCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALimitBreakCamera::SetTargetActor(AActor* Actor)
{
	TargetActor = Actor;

	FVector MyLoc = GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();
	SetActorRotation(TargetActor->GetActorRotation());
	
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
	CachedRotation = LookRot;
	SetActorRotation(LookRot);
}

