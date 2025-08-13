// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/BaseSkeletalMeshWeapon.h"

#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABaseSkeletalMeshWeapon::ABaseSkeletalMeshWeapon() : ABaseWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	TrailVfx = CreateDefaultSubobject<UNiagaraComponent>(FName("WeaponVFX"));
	TrailVfx->SetupAttachment(RootComponent);
}
