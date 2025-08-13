// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/TargetingUI.h"

void UTargetingUI::SetTarget(AActor* Target)
{
	TargetActor = Target;
}

void UTargetingUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();

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
