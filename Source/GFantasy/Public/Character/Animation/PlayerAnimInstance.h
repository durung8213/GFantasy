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
	//초기화 시 Pawns와 캐릭터 레퍼런스 캐싱
	virtual void NativeInitializeAnimation() override;

	// 매 프레임 애니메이션 프로퍼티 업데이트
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 이동 방향 각도 계산 ( Actor 전방 대비)
	UFUNCTION(BlueprintCallable, Category = "Animation | Movement")
	float CalculateMovementDirection(const FVector& Velocity, const FRotator& BaseRotation) const;


protected:
	//캐싱된 주인 Pawn 과 Character
	UPROPERTY(Transient)
	APawn* PawnOwner;

	UPROPERTY(Transient)
	ACharacter* CharacterOwner;


	// 이동 속도( 수평 )
	UPROPERTY(BlueprintReadOnly, Category = "Animation | Movement")
	float Speed;

	// 입력 가속 여부
	UPROPERTY(BlueprintReadOnly, Category = "Animation | Movement")
	bool bIsAccelerating;

	// 이동 방향 각도 ( -180 ~ 180 )
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
