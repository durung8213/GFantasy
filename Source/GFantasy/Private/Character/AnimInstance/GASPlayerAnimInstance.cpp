// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/AnimInstance/GASPlayerAnimInstance.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"


UGASPlayerAnimInstance::UGASPlayerAnimInstance()
{
}

void UGASPlayerAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UGASPlayerAnimInstance, bHasUpperBodyAction);
	DOREPLIFETIME(UGASPlayerAnimInstance, bIsMounted);
	DOREPLIFETIME(UGASPlayerAnimInstance, bIsDead);
	DOREPLIFETIME(UGASPlayerAnimInstance, bIsLockOn);
	DOREPLIFETIME(UGASPlayerAnimInstance, bIsInCombat);
	DOREPLIFETIME(UGASPlayerAnimInstance, bHasWeapon);
	DOREPLIFETIME(UGASPlayerAnimInstance, bUseUpperSlot);
	DOREPLIFETIME(UGASPlayerAnimInstance, bIsSprint);
	DOREPLIFETIME(UGASPlayerAnimInstance, bIsRespawned);
}

void UGASPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (APawn* OwnerPawn = TryGetPawnOwner())
	{
		if (ACharacter* Char =Cast<ACharacter>(OwnerPawn))
		{
			MainMesh = Char->GetMesh();
		}
	}
}


