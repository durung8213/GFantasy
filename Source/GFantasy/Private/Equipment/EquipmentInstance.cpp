// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Equipment/EquipmentActor.h"
#include "Equipment/EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "Character/GASPlayerCharacter.h"
#include "Character/Weapon/WeaponBase.h"
#include "Controller/GASWorldPlayerController.h"

void UEquipmentInstance::OnEquipped()
{
	
}

void UEquipmentInstance::OnUnEquipped()
{
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentActorsToSpawn>& ActorsToSpawn)
{
	if (ACharacter* OwningCharacter = GetCharacter())
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();
		TWeakObjectPtr<UEquipmentInstance> WeakThis(this);
		
		for (const FEquipmentActorsToSpawn& ActorToSpawn : ActorsToSpawn)
		{
			if (IsValid(ActorToSpawn.EquipmentClass.Get()))
			{
				AEquipmentActor* NewActor = GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(), FTransform::Identity,
					OwningCharacter);

				NewActor->FinishSpawning(FTransform::Identity);
				NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);
				SpawnedActors.Emplace(NewActor);

				AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetCharacter());
				AWeaponBase* Weapon = Cast<AWeaponBase>(NewActor);

				if (Player)
				{
					if (Weapon)
					{
						Player->CurrentWeapon = Weapon;
						UE_LOG(LogTemp, Warning, TEXT("Weapon Save To Player"));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("NO Weapon"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("No Player"));
				}

			}
			else
			{
				Manager.RequestAsyncLoad(ActorToSpawn.EquipmentClass.ToSoftObjectPath(),
					[WeakThis, ActorToSpawn, OwningCharacter]
					{
						AEquipmentActor* NewActor = WeakThis->GetWorld()->SpawnActorDeferred<AEquipmentActor>(ActorToSpawn.EquipmentClass.Get(), FTransform::Identity, OwningCharacter);

						NewActor->FinishSpawning(FTransform::Identity);
						NewActor->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachName);
						WeakThis->SpawnedActors.Emplace(NewActor);

					});

				AGASPlayerCharacter* Player = Cast<AGASPlayerCharacter>(GetCharacter());	
				if (Player)
				{
					Player->CurrentWeapon = nullptr;
				}

			}
		}

		

	}
}

void UEquipmentInstance::DestroySpawnedActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		Actor->Destroy();
	}
}

ACharacter* UEquipmentInstance::GetCharacter()
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetOuter()))
	{
		return Cast<ACharacter>(PlayerController->GetPawn());
	}

	return nullptr;
}
