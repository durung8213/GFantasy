// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/BaseStaticMeshWeapon.h"

#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"

ABaseStaticMeshWeapon::ABaseStaticMeshWeapon() : ABaseWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	TrailVfx = CreateDefaultSubobject<UNiagaraComponent>(FName("WeaponVFX"));
	TrailVfx->SetupAttachment(RootComponent);
}

FTransform ABaseStaticMeshWeapon::GetWeaponSocketWorldTransform(FName Socket)
{
	return WeaponMesh->GetSocketTransform(Socket);
}

