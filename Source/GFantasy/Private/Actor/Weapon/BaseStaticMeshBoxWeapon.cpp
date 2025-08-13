// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/BaseStaticMeshBoxWeapon.h"

#include "Components/BoxComponent.h"


ABaseStaticMeshBoxWeapon::ABaseStaticMeshBoxWeapon(): ABaseStaticMeshWeapon()
{
	HitCollision = CreateDefaultSubobject<UBoxComponent>(FName("HitCollision"));
	HitCollision->SetupAttachment(RootComponent);

	HitComponents.Add(HitCollision);
}
