// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/Player/FocusingUI.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"

void UFocusingUI::SetTarget(AActor* Target)
{
	TargetActor = Target;
}

void UFocusingUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	UpdatePosition(PC);
}

void UFocusingUI::UpdatePosition(APlayerController* PC)
{
	if (!TargetActor || !PC)
		return;

	FVector WorldCenter = TargetActor->GetActorLocation();

	if (const USkeletalMeshComponent* Mesh = TargetActor->FindComponentByClass<USkeletalMeshComponent>())
	{
		WorldCenter = Mesh->Bounds.Origin;
	}

	FVector2D ScreenPos;

	if (PC->ProjectWorldLocationToScreen(WorldCenter, ScreenPos))
	{
		ScreenPos.X = FMath::RoundToFloat(ScreenPos.X);
		ScreenPos.Y = FMath::RoundToFloat(ScreenPos.Y);

		SetPositionInViewport(ScreenPos, true);
		SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	}
}
