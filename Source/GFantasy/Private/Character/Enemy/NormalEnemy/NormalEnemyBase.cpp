// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/NormalEnemy/NormalEnemyBase.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Controller/AIControllerBase.h"
#include "Controller/GASWorldPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AIPerceptionSystem.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Quest/KillCountQuest.h"
#include "Quest/QuestManagerComponent.h"

ANormalEnemyBase::ANormalEnemyBase()
{
	AreaCollision = CreateDefaultSubobject<USphereComponent>("AreaCollision");
	AreaCollision->SetupAttachment(RootComponent);
	AreaCollision->SetSphereRadius(1200.f);

	AreaCollision->OnComponentBeginOverlap.AddDynamic(this, &ANormalEnemyBase::AreaCollisionBeginOverlap);
	AreaCollision->OnComponentEndOverlap.AddDynamic(this, &ANormalEnemyBase::AreaCollisionEndOverlap);
}

void ANormalEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	AAIControllerBase* CurrentController = Cast<AAIControllerBase>(GetController());
	if (CurrentController)
	{
		CurrentController->SpawnerLocation = SpawnerLocation;
		CurrentTargetLocation = GetActorLocation();
		CurrentController->SetCurrentTargetLocation(CurrentTargetLocation);
		CurrentController->SetControlledPawn();
	}
	
	ClientWidgetHide();
}

void ANormalEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ANormalEnemyBase, bIsRandomIdle, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ANormalEnemyBase, RandomIdleIndex, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ANormalEnemyBase, CurrentTargetLocation, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ANormalEnemyBase, bIsTurningLeftInPlace, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ANormalEnemyBase, bIsTurningRightInPlace, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ANormalEnemyBase, CurrentFocusActor, COND_None, REPNOTIFY_Always);
}

void ANormalEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick 함수 내에 작성
	FVector ActorLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector VelocityDir = GetVelocity().GetSafeNormal();

#if WITH_EDITOR
	// 회전 방향 (보통 파란색)
	DrawDebugDirectionalArrow(GetWorld(), ActorLocation, ActorLocation + ForwardVector * 300, 100.f, FColor::Blue, false, -1.f, 0, 3.f);

	// 이동 방향 (보통 빨간색)
	DrawDebugDirectionalArrow(GetWorld(), ActorLocation, ActorLocation + VelocityDir * 300, 100.f, FColor::Red, false, -1.f, 0, 3.f);

#endif
}

void ANormalEnemyBase::AreaCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* CurrentPawn = Cast<ACharacter>(OtherActor))
	{
		if (AGASWorldPlayerController* CurrentController = Cast<AGASWorldPlayerController>(CurrentPawn->GetController()))
		{
			SetOwner(CurrentController);
		
			ServerInitHealths();
			ClientWidgetVisible(ESlateVisibility::Visible);
		}
	}
	
}


void ANormalEnemyBase::AreaCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* CurrentPawn = Cast<ACharacter>(OtherActor))
	{
		if (AGASWorldPlayerController* CurrentController = Cast<AGASWorldPlayerController>(CurrentPawn->GetController()))
		{
			SetOwner(CurrentController);
		
			ServerInitHealths();
			ClientWidgetVisible(ESlateVisibility::Hidden);
		}
	}
}

void ANormalEnemyBase::OnNormalDamage(const FGameplayEffectSpec& EffectSpec,
                                      const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	Super::OnNormalDamage(EffectSpec, EvaluatedData, Target);
	AActor* HitActor = EffectSpec.GetContext().GetInstigator();
	UAISense_Damage::ReportDamageEvent(
		GetWorld(),
		this,
		HitActor,
		10,
		HitActor->GetActorLocation(),
		GetActorLocation()
	);
	 
}

void ANormalEnemyBase::OnStrongDamage(const FGameplayEffectSpec& EffectSpec,
	const FGameplayModifierEvaluatedData& EvaluatedData, const UAbilitySystemComponent* Target)
{
	Super::OnStrongDamage(EffectSpec, EvaluatedData, Target);

	AActor* HitActor = EffectSpec.GetContext().GetInstigator();
	UAISense_Damage::ReportDamageEvent(
		GetWorld(),
		this,
		HitActor,
		10,
		HitActor->GetActorLocation(),
		GetActorLocation()
	);
}

void ANormalEnemyBase::RandomIdleStart()
{
	bIsRandomIdle = true;

	RandomIdleIndex = FMath::RandRange(0, MaxRandomIdleIndex);

	if (AAIControllerBase* CurrentController = Cast<AAIControllerBase>(GetController()))
	{
		CurrentController->GetBlackboardComponent()->SetValueAsVector(FName("PassiveLocation"), GetActorLocation());
	}
}

void ANormalEnemyBase::RandomIdleEnd()
{
	bIsRandomIdle = false;
	OnRandomIdleEnd.Broadcast();
}

FVector ANormalEnemyBase::GetCurrentTargetLocation()
{
	return CurrentTargetLocation;
}

void ANormalEnemyBase::SetCurrentTargetLocation(FVector NewTargetLocation)
{
	CurrentTargetLocation = NewTargetLocation;
}

void ANormalEnemyBase::SetControllerFocusActor_Implementation(AActor* NewFocusActor)
{
	Super::SetControllerFocusActor_Implementation(NewFocusActor);
	CurrentFocusActor = NewFocusActor;
}

AActor* ANormalEnemyBase::GetControllerFocusActor_Implementation()
{
	return CurrentFocusActor;
}

void ANormalEnemyBase::ClientWidgetHide_Implementation()
{
	if (EnemyStateWidgetComponent && EnemyStateWidgetComponent->GetWidget())
	{
		EnemyStateWidgetComponent->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
		EnemyStateWidgetComponent->SetActive(false);
	}
	else
	{
		FTimerHandle WidgetSetTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
		WidgetSetTimerHandle,
		this,
		&ANormalEnemyBase::ClientWidgetHide,
		0.2f,
		false
		);
	}
}

void ANormalEnemyBase::MultiWidgetHide_Implementation()
{
	if (EnemyStateWidgetComponent && EnemyStateWidgetComponent->GetWidget())
	{
		//EnemyStateWidgetComponent->GetWidget()->SetVisibility(VisibleMode);
		EnemyStateWidgetComponent->SetActive(false);
	}
	else
	{
		FTimerHandle WidgetSetTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
		WidgetSetTimerHandle,
		this,
		&ANormalEnemyBase::MultiWidgetHide,
		0.2f,
		false
		);
	}
}


void ANormalEnemyBase::ClientWidgetVisible_Implementation(ESlateVisibility VisibleMode)
{

	if (bIsDead)
	{
		return;
	}

	if (EnemyStateWidgetComponent && EnemyStateWidgetComponent->GetWidget())
	{
		//EnemyStateWidgetComponent->GetWidget()->SetVisibility(VisibleMode);
		//EnemyStateWidgetComponent->GetWidget()->SetRenderOpacity(1);

		if (VisibleMode == ESlateVisibility::Visible)
		{
			GetWorld()->GetTimerManager().ClearTimer(WidgetDissolveTimerHandle);
			//EnemyStateWidgetComponent->GetWidget()->SetRenderOpacity(1);
			EnemyStateWidgetComponent->SetVisibility(true);
			EnemyStateWidgetComponent->GetWidget()->SetRenderOpacity(1);
			EnemyStateWidgetComponent->GetWidget()->SetVisibility(VisibleMode);


		}
		else if (VisibleMode == ESlateVisibility::Hidden)
		{
			DissolveEnemyStateWidget();
		}
	}
	else
	{
		FTimerHandle WidgetSetTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
		WidgetSetTimerHandle,
		[this,VisibleMode]
		{
			ANormalEnemyBase::ClientWidgetVisible(VisibleMode);
		},
		0.2f,
		false
		);
	}
	
}


void ANormalEnemyBase::MultiPlayMontage_Implementation(UAnimMontage* Montage)
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(Montage);
	}
}
