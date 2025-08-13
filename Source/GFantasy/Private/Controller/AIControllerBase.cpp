// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/AIControllerBase.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GASCombatNPC.h"
#include "Character/Enemy/NormalEnemy/NormalEnemyBase.h"
#include "Enum/EnemyEnumLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

DEFINE_LOG_CATEGORY(LOG_AIControllerBase);

AAIControllerBase::AAIControllerBase()
{
	// 퍼셉션 컴포넌트 추가
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");

	// 시야 추가
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	// if (SightConfig)
	// {
	// 	// Perception 컴포넌트에 시야 설정 추가
	// 	AIPerception->ConfigureSense(*SightConfig);
	//
	// 	// 주 감지 센스를 시야로 설정
	// 	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Failed to create SightConfig"));
	// }

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	if (HearingConfig)
	{
		AIPerception->ConfigureSense(*HearingConfig);
	}
	
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	if (DamageConfig)
	{
		DamageConfig->SetMaxAge(8.0f); 
		AIPerception->ConfigureSense(*DamageConfig);
	}
	
	
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		SetEnemyState(EEnemyState::Passive);
	}
	
	if (bUseAIPerception)
	{
		if (!AIPerception->OnPerceptionUpdated.IsAlreadyBound(this, &AAIControllerBase::OnPerceptionUpdate))
		{
			AIPerception->OnPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnPerceptionUpdate);
		}
		
	}

	
	if (GetPawn())
	{
		SetCurrentTargetLocation(GetPawn()->GetActorLocation());
		if (Blackboard)
		{
			Blackboard->SetValueAsVector(FName("PassiveLocation"), GetPawn()->GetActorLocation());
		}
		
	}
}

void AAIControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AAIControllerBase, CurrentTargetLocation, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AAIControllerBase, bRotateToFocusActor, COND_None, REPNOTIFY_Always);
}

void AAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	// DrawDebugSphere(
	// 	GetWorld(),
	// 	CurrentTargetLocation,           // 위치
	// 	50.0f,                     // 반지름
	// 	12,                         // 세그먼트 수
	// 	FColor::Green,             // 색상
	// 	false,                     // 영구 여부
	// 	5.0f                       // 지속 시간
	// );

#endif

}

void AAIControllerBase::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	Super::SetFocus(NewFocus, InPriority);

	//UKismetSystemLibrary::PrintString(this, TEXT("포커스 셋 됨"));
	
	if (AActor* FocusedPawn = GetPawn())
	{
		if (FocusedPawn && FocusedPawn->GetClass()->ImplementsInterface(UEnemyInterface::StaticClass()))
		{
			IEnemyInterface::Execute_SetbIsFocus(FocusedPawn, true);
			
		}
	}

	Blackboard->SetValueAsObject(FName("FocusActor"), NewFocus);
	
}

void AAIControllerBase::ClearFocus(EAIFocusPriority::Type InPriority)
{
	Super::ClearFocus(InPriority);

	//UKismetSystemLibrary::PrintString(this, TEXT("포커스 클리어 됨"));
	AActor* FocusedPawn = GetPawn();
	if (InPriority != EAIFocusPriority::Move && FocusedPawn)
	{
		if (FocusedPawn && FocusedPawn->GetClass()->ImplementsInterface(UEnemyInterface::StaticClass()))
		{
			IEnemyInterface::Execute_SetbIsFocus(FocusedPawn, false);
			//Blackboard->SetValueAsObject(FName("FocusActor"), nullptr);
		}
	}
	
}

void AAIControllerBase::StopBT()
{
	GetBrainComponent()->StopLogic(FString("AIDead"));
}

void AAIControllerBase::SetEnemyState(EEnemyState NewState)
{
	CurrentState = NewState;
	if (Blackboard)
	{
		Blackboard->SetValueAsEnum(StateKeyName, static_cast<uint8>(NewState));
		
	}
	
	
}

EEnemyState AAIControllerBase::GetEnemyState()
{

	if (Blackboard)
	{
		EEnemyState CurrentBlackboardState = static_cast<EEnemyState>(Blackboard->GetValueAsEnum(StateKeyName));
		if (CurrentBlackboardState != CurrentState)
		{
			UE_LOG(LOG_AIControllerBase, Error, TEXT("AIController와 캐릭터 간의 상태가 일치하지 않습니다 수정이 필요합니다"));
			//UKismetSystemLibrary::PrintString(this, TEXT("AIController와 캐릭터 간의 상태가 일치하지 않습니다 수정이 필요합니다"));
		}
		return CurrentBlackboardState;
	}

	return CurrentState;
}

void AAIControllerBase::OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* UpdatedActor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo ActorPerceptionInfos = FActorPerceptionBlueprintInfo();
		if (AIPerception->GetActorsPerception(UpdatedActor, ActorPerceptionInfos))
		{
			for (FAIStimulus AIStimulus : ActorPerceptionInfos.LastSensedStimuli)
			{
				// 어떤 클래스로 감지했는지 알기
				TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), AIStimulus);

				if (!SenseClass)
				{
					continue;
				}

				if (bUseSight)
				{
					if (SenseClass == UAISense_Sight::StaticClass())
					{
						if (AIStimulus.WasSuccessfullySensed())
						{
							//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s가 시야로 감지됨"), *UpdatedActor->GetName()));
							OnEntityDetectedBySense(UpdatedActor);
						}
						else
						{
							//SetEnemyState(EEnemyState::Passive);
							//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s가 시야 감각에서 잊혀짐"), *UpdatedActor->GetName()));
						}
						
					}
					
				}
				if (bUseHearing)
				{
					if (SenseClass == UAISense_Hearing::StaticClass())
					{
						if (AIStimulus.WasSuccessfullySensed())
						{
							//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s가 낸 소리가 감지됨"), *UpdatedActor->GetName()));
							OnEntityDetectedByHearing(UpdatedActor);
						}
						else
						{
							//SetEnemyState(EEnemyState::Passive);
							//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s가 소리감각에서 잊혀짐"), *UpdatedActor->GetName()));
						}
						
					}
				}
				if (bUseDamage)
				{
					if (SenseClass == UAISense_Damage::StaticClass())
					{
						if (AIStimulus.WasSuccessfullySensed())
						{
							OnEntityDetectedByDamage(UpdatedActor);
							//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s가 AI에게 데미지를 입힘"), *UpdatedActor->GetName()));
						}
						else
						{
							//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s가 감각에서 잊혀짐"), *UpdatedActor->GetName()));
						}
					
					}
				}
				else
				{
					//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s가 설정한 센서를 제외한 다른 센서를 통해 감지됨"), *UpdatedActor->GetName()));
				}
			}
		}
	}
	
}

void AAIControllerBase::OnEntityDetectedBySense_Implementation(AActor* Actor)
{
	
}

void AAIControllerBase::OnEntityDetectedByHearing_Implementation(AActor* Actor)
{
	
}

void AAIControllerBase::OnEntityDetectedByDamage_Implementation(AActor* Actor)
{
	
}

void AAIControllerBase::SetCurrentTargetLocation(FVector NewCurrentTargetLocation)
{
	CurrentTargetLocation = NewCurrentTargetLocation;
	ANormalEnemyBase* CurrentPawn = Cast<ANormalEnemyBase>(GetPawn());
	if (CurrentPawn)
	{
		CurrentPawn->SetCurrentTargetLocation(CurrentTargetLocation);
	}
}

FVector AAIControllerBase::GetCurrentTargetLocation()
{
	return CurrentTargetLocation;
}

void AAIControllerBase::SetControlledPawn()
{
	CurrentCombatPawn = Cast<AGASCombatNPC>(GetPawn());
	CurrentNormalEnemyPawn = Cast<ANormalEnemyBase>(GetPawn());
	
	
}

void AAIControllerBase::RotateToPlayerStart()
{
	float Time = GetWorld()->GetDeltaSeconds();
	
	GetWorld()->GetTimerManager().SetTimer(
		RotateToPlayerTimerHandle,
		[this,Time]
		{
			RotateToPlayer(Time);
		},
		Time,
		true
	);
}

void AAIControllerBase::RotateToPlayer(float WorldDeltaTime)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("RotateToPlayer 실행중"));
	if (CurrentNormalEnemyPawn)
	{
		FRotator CurrentRot = CurrentNormalEnemyPawn->GetActorRotation();
		if (GetFocusActor())
		{
			FVector Direction = (GetFocusActor()->GetActorLocation() - CurrentNormalEnemyPawn->GetActorLocation()).GetSafeNormal();
			FRotator TargetRot = Direction.Rotation();
			FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, WorldDeltaTime, 3.f);
			CurrentNormalEnemyPawn->SetActorRotation(NewRot);

			float YawDelta = FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, TargetRot.Yaw);

			if (YawDelta > 25)
			{
				// 오른쪽(시계 방향)으로 회전
				CurrentNormalEnemyPawn->bIsTurningRightInPlace = true;
			}
			else if (YawDelta < -25)
			{
				// 왼쪽(반시계 방향)으로 회전
				CurrentNormalEnemyPawn->bIsTurningLeftInPlace = true;
			}
			else
			{
				CurrentNormalEnemyPawn->bIsTurningRightInPlace = false;
				CurrentNormalEnemyPawn->bIsTurningLeftInPlace = false;
				RotateToPlayerEnd();
				// 정면
			}
		}
		else
		{
			CurrentNormalEnemyPawn->bIsTurningRightInPlace = false;
			CurrentNormalEnemyPawn->bIsTurningLeftInPlace = false;
			RotateToPlayerEnd();
		}
		
	}
	
}

void AAIControllerBase::RotateToPlayerEnd()
{
	GetWorldTimerManager().ClearTimer(RotateToPlayerTimerHandle);
	OnRotateToPlayerEnd.Broadcast();
}

void AAIControllerBase::ChangeUseSence(bool NewUseSight, bool NewUseHearing, bool NewUseDamage)
{
	bUseSight = NewUseSight;
	bUseHearing = NewUseHearing;
	bUseDamage = NewUseDamage;
}

