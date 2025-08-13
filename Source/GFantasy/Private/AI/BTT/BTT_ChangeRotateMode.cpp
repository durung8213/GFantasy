// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT/BTT_ChangeRotateMode.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UBTT_ChangeRotateMode::UBTT_ChangeRotateMode()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTT_ChangeRotateMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (OwnerComp.GetAIOwner())
	{
		if (ACharacter* CurrentCharacter = Cast<ACharacter>( OwnerComp.GetAIOwner()->GetPawn())) 
		{
			UCharacterMovementComponent* CurrentMovementMode = CurrentCharacter->GetCharacterMovement();
			if (CurrentMovementMode)
			{
				switch (FocusMode)
				{
				case EEnemyRotateMode::RoundMode:
					CurrentMovementMode->bUseControllerDesiredRotation = true;
					CurrentCharacter->bUseControllerRotationYaw = false;
					CurrentMovementMode->bOrientRotationToMovement = false;
					break;

				case EEnemyRotateMode::FocusMode:
					CurrentMovementMode->bUseControllerDesiredRotation = false;
					CurrentCharacter->bUseControllerRotationYaw = true;
					CurrentMovementMode->bOrientRotationToMovement = true;
					break;
			
				default:
					UE_LOG(LogTemp, Error, TEXT("Invalid focus mode specified"));
					//UKismetSystemLibrary::PrintString(this, TEXT("UBTT_ChangeRotateMode 이럴 수가 있나?"));
				}

				return EBTNodeResult::Succeeded;
			}
			return EBTNodeResult::Failed;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
	else
	{
		return EBTNodeResult::Failed;
	}
}
