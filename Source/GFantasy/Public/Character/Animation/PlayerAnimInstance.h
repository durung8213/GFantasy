// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GFANTASY_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	//�ʱ�ȭ �� Pawns�� ĳ���� ���۷��� ĳ��
	virtual void NativeInitializeAnimation() override;

	// �� ������ �ִϸ��̼� ������Ƽ ������Ʈ
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// �̵� ���� ���� ��� ( Actor ���� ���)
	UFUNCTION(BlueprintCallable, Category = "Animation | Movement")
	float CalculateMovementDirection(const FVector& Velocity, const FRotator& BaseRotation) const;


protected:
	//ĳ�̵� ���� Pawn �� Character
	UPROPERTY(Transient)
	APawn* PawnOwner;

	UPROPERTY(Transient)
	ACharacter* CharacterOwner;


	// �̵� �ӵ�( ���� )
	UPROPERTY(BlueprintReadOnly, Category = "Animation | Movement")
	float Speed;

	// �Է� ���� ����
	UPROPERTY(BlueprintReadOnly, Category = "Animation | Movement")
	bool bIsAccelerating;

	// �̵� ���� ���� ( -180 ~ 180 )
	UPROPERTY(BlueprintReadOnly, Category = "Animation | Movement")
	float MovementDirection;
	
	UPROPERTY(BlueprintReadOnly, Category = "Animation | Movement")
	float MoveX;

	UPROPERTY(BlueprintReadOnly, Category = "Animation | Movement")
	float MoveY;

	UPROPERTY(BlueprintReadOnly, Category = "LockOn")
	bool bIsLockOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	UBlendSpace* FreeMoveBlendSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	UBlendSpace* LockOnBlendSpace;
};
