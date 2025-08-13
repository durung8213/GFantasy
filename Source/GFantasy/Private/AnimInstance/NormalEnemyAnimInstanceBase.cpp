// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/NormalEnemyAnimInstanceBase.h"

#include "Character/Enemy/NormalEnemy/NormalEnemyBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UNormalEnemyAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (CurrentPawn)
	{
		CurrentNormalEnemyPawn = Cast<ANormalEnemyBase>(CurrentPawn);	
	}
	
}

void UNormalEnemyAnimInstanceBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CurrentNormalEnemyPawn)
	{
		bIsRandomIdle = CurrentNormalEnemyPawn->bIsRandomIdle;
		
		SetRotateValue();
		
	}
	
	
}

int UNormalEnemyAnimInstanceBase::GetRandomIdleIndex()
{
	if (CurrentNormalEnemyPawn)
	{
		return CurrentNormalEnemyPawn->RandomIdleIndex;
	}
	return 0;
}

void UNormalEnemyAnimInstanceBase::SetRotateValue()
{
	if (CurrentNormalEnemyPawn)
	{
		// 1. 회전 플래그 설정
		bRotateL = false;
		bRotateR = false;
		
		FVector ActorLocation = CurrentNormalEnemyPawn->GetActorLocation();
		FVector ForwardVector = CurrentNormalEnemyPawn->GetActorForwardVector();
		FVector VelocityDir = CurrentNormalEnemyPawn->GetVelocity().GetSafeNormal();
	
		if (!VelocityDir.IsNearlyZero())
		{
			// 벡터 사이의 내적을 사용해서 각도를 구함 (라디안 기준)
			float Dot = FVector::DotProduct(ForwardVector, VelocityDir);
			float AngleRad = FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f));
			float AngleDeg = FMath::RadiansToDegrees(AngleRad);

			if (AngleDeg >= 30.f)
			{
				// 방향 확인을 위해 CrossProduct의 Z축 확인
				float CrossZ = FVector::CrossProduct(ForwardVector, VelocityDir).Z;

				if (CrossZ > 0)
				{
					bRotateR = true; // 왼쪽 회전
				}
				else
				{
					bRotateL = true; // 오른쪽 회전
				}

				//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AngleDegrees : %lf, Dir : %lf"), AngleDeg, CrossZ), true, true, FLinearColor(0.0f, 0.66f, 1.0f), 0.03f);
			}
		}
		
		bIsTurningLeftInPlace = CurrentNormalEnemyPawn->bIsTurningLeftInPlace;
		bIsTurningRightInPlace = CurrentNormalEnemyPawn->bIsTurningRightInPlace;
		

	}
}
