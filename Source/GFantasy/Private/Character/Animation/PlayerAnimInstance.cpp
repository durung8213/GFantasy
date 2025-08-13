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

	// 1. �ӵ�(����)
	const FVector Velocity = CharacterOwner->GetVelocity();
	const FVector LateralVel = FVector(Velocity.X, Velocity.Y, 0.f);
	Speed = LateralVel.Size();

	// ���� ����
	bIsAccelerating = CharacterOwner->GetCharacterMovement()->GetCurrentAcceleration().SizeSquared() > 0.f;

	// �̵� ����
	MovementDirection = CalculateMovementDirection(Velocity, PawnOwner->GetActorRotation());

	//BlendSpace X/Y ���
	//MovementDirection�� Actor ����� �ӵ� ���� ���� ����
	// �̸� �������� ��ȯ ��, �ڻ���/������ �����ָ� ����/ ���� �ӵ� ������ ����

	const float Rad = FMath::DegreesToRadians(MovementDirection);
	MoveX = FMath::Cos(Rad) * Speed;
	MoveY = FMath::Sin(Rad) * Speed;

	if (auto* GASChar = Cast<AGASPlayerCharacter>(CharacterOwner))
	{
		// ���� ���¸� �����´�.
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

	// Velocity ���� ������ ����Ͽ� Actor ������� ���� ��ȯ
	const float VelocityAngle = FMath::Atan2(LateralVel.Y, LateralVel.X) * (180.f / PI);
	const float Delta = FMath::FindDeltaAngleDegrees(BaseRotation.Yaw, VelocityAngle);
	return Delta;
}
