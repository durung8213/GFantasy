// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffect/GameplayEffectExecutionCalculation/GEC_SendEventToInstigator.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UGEC_SendEventToInstigator::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	// Instigator Actor 가져오기
	AActor* InstigatorActor = ExecutionParams.GetSourceAbilitySystemComponent()->GetOwnerActor();
	if (!InstigatorActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("InstigatorActor is null"));
		return;
	}

	// Gameplay Event Data 구성
	FGameplayEventData EventData;
	EventData.Instigator = InstigatorActor;
	EventData.EventTag = EventTag;
	EventData.Target = InstigatorActor;


	
	// Instigator의 ASC 가져오기
	UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InstigatorActor);
	if (!InstigatorASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Instigator ASC is null"));
		return;
	}
	
	// GameplayEvent 전송
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstigatorActor, EventTag, EventData);
	//InstigatorASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}
