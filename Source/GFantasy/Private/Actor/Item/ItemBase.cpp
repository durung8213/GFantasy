// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Item/ItemBase.h"

#include "NiagaraComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Controller/GASWorldPlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));

	RootComponent = BoxCollision;

	ItemSpawnNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SphereComponent"));

	ItemSpawnNiagaraComponent->SetupAttachment(BoxCollision);
	
}

void AItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AItemBase, ItemTag, COND_None, REPNOTIFY_Always);
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnOverlapBegin);
	
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if (HasAuthority())
	// {
	// 	AGASPlayerCharacter* CurrentPlayer = Cast<AGASPlayerCharacter>(OtherActor);
	// 	if (CurrentPlayer)
	// 	{
	// 		AGASWorldPlayerController* CurrentController = Cast<AGASWorldPlayerController>( CurrentPlayer->GetController());
	// 		if (CurrentController)
	// 		{
	// 			IInventoryInterface* CurrentInventoryInterface = Cast<IInventoryInterface>(CurrentController);
	// 			UInventoryComponent* CurrentInventoryComponent = CurrentInventoryInterface->GetInventoryComponent();
	// 			if (CurrentInventoryComponent)
	// 			{
	// 				CurrentInventoryComponent->AddItem(ItemTag, 1);
	// 			}
	// 		}
	// 	}
	// }
	
	
}

