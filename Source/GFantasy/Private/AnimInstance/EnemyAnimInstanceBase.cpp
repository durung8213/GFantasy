// Fill out your copyright notice in the Description page of Project Settings.CurrentPawn


#include "AnimInstance/EnemyAnimInstanceBase.h"

#include "KismetAnimationLibrary.h"
#include "Character/GASCombatNPC.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UEnemyAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CurrentPawn = Cast<AGASCombatNPC>(GetOwningActor());
	if (CurrentPawn)
	{
		MovementComponent = CurrentPawn->GetCharacterMovement();
	}
	
}

void UEnemyAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CurrentPawn && MovementComponent)
	{
		Velocity = MovementComponent->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		bShouldMove = !MovementComponent->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.0f) && GroundSpeed > 3.0f;
		bIsFalling = MovementComponent->IsFalling();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity,CurrentPawn->GetActorRotation());
		bIsFocus = CurrentPawn->GetbIsFocus();
		//bIsFocus = IEnemyInterface::Execute_GetbIsFocus(CurrentPawn);
	}
	else if (GetWorld()->IsGameWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("No Pawn or Character"));
		//UKismetSystemLibrary::PrintString(this, TEXT("애니메이션 블루프린트에서 폰이나 캐릭터 없음"));
	}
}
