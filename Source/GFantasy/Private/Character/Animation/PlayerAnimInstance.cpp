// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Animation/PlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GASPlayerCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (PawnOwner)
	{
		CharacterOwner = Cast<ACharacter>(PawnOwner);
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!PawnOwner)
	{
		PawnOwner = TryGetPawnOwner();
	}
	if (!CharacterOwner && PawnOwner)
	{
		CharacterOwner = Cast<ACharacter>(PawnOwner);
	}
	if (!CharacterOwner)
	{
		return;
	}

	// 1. 속도(수평)
	const FVector Velocity = CharacterOwner->GetVelocity();
	const FVector LateralVel = FVector(Velocity.X, Velocity.Y, 0.f);
	Speed = LateralVel.Size();

	// 가속 여부
	bIsAccelerating = CharacterOwner->GetCharacterMovement()->GetCurrentAcceleration().SizeSquared() > 0.f;

	// 이동 방향
	MovementDirection = CalculateMovementDirection(Velocity, PawnOwner->GetActorRotation());

	//BlendSpace X/Y 계산
	//MovementDirection은 Actor 전방과 속도 벡터 사이 각도
	// 이를 라디안으로 변환 후, 코사인/사인을 곱해주면 전진/ 측면 속도 성분이 나옴

	const float Rad = FMath::DegreesToRadians(MovementDirection);
	MoveX = FMath::Cos(Rad) * Speed;
	MoveY = FMath::Sin(Rad) * Speed;

	if (auto* GASChar = Cast<AGASPlayerCharacter>(CharacterOwner))
	{
		// 락온 상태를 가져온다.
		bIsLockOn = (GASChar->TargetingState == ETargetingState::LockOn);
	}
	else
	{
		bIsLockOn = false;
	}
}

float UPlayerAnimInstance::CalculateMovementDirection(const FVector& Velocity, const FRotator& BaseRotation) const
{
	const FVector LateralVel = FVector(Velocity.X, Velocity.Y, 0.f);
	if (LateralVel.IsNearlyZero())
	{
		return 0.0f;
	}

	// Velocity 벡터 각도를 계산하여 Actor 전방과의 차이 반환
	const float VelocityAngle = FMath::Atan2(LateralVel.Y, LateralVel.X) * (180.f / PI);
	const float Delta = FMath::FindDeltaAngleDegrees(BaseRotation.Yaw, VelocityAngle);
	return Delta;
}
