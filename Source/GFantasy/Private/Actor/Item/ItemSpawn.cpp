// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Item/ItemSpawn.h"

#include "NiagaraComponent.h"
#include "Actor/Item/ItemBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AItemSpawn::AItemSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	RootComponent = SphereComponent;

	ItemSpawnNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemSpawnNiagaraComponent"));

	ItemSpawnNiagaraComponent->SetupAttachment(RootComponent);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");

	
	
}

void AItemSpawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AItemSpawn, ItemTag, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AItemSpawn, bRandomSpawn, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AItemSpawn, RandomSpawnVector, COND_None, REPNOTIFY_Always);
}

// Called when the game starts or when spawned
void AItemSpawn::BeginPlay()
{
	Super::BeginPlay();

	if (bRandomSpawn)
	{
		ProjectileMovement->Velocity = FVector( FMath::RandRange(-50.0f, 50.0f),
		FMath::RandRange(-50.0f, 50.0f),
		FMath::RandRange(300.f, 400.0f)
		);
	}
	else
	{
		ProjectileMovement->Velocity = RandomSpawnVector;
	}
	
	SphereComponent->OnComponentHit.AddDynamic(this, &AItemSpawn::OnHit);
	
}

// Called every frame
void AItemSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemSpawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// if (HasAuthority())
	// {
	// 	FActorSpawnParameters SpawnParams;
	// 	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	// 	GetWorld()->SpawnActor<AItemBase>(AItemBase::StaticClass(), GetActorLocation(), FRotator());
	// 	Destroy(true);
	// }
}

