// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/Weapon/SwordWeapon.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

ASwordWeapon::ASwordWeapon()
{
	LimitBreakVFX_Sword = CreateDefaultSubobject<UNiagaraComponent>(FName("LimitBreakVFX_Sword"));
	LimitBreakVFX_Sword->SetupAttachment(RootComponent);
	
	LimitBreakVFX_Trail = CreateDefaultSubobject<UNiagaraComponent>(FName("LimitBreakVFX_Player"));
	LimitBreakVFX_Trail->SetupAttachment(RootComponent);
}

void ASwordWeapon::ActivateLimitVFX()
{
	if (LimitBreakVFX_Sword)
	{
		LimitBreakVFX_Sword->Activate();
	}

	if (LimitBreakVFX_Trail)
	{
		LimitBreakVFX_Trail->Activate();
	}
}

void ASwordWeapon::DeactivateLimitVFX()
{
	if (LimitBreakVFX_Sword)
	{
		LimitBreakVFX_Sword->Deactivate();
	}

	if (LimitBreakVFX_Trail)
	{
		LimitBreakVFX_Trail->Deactivate();
	}
}

void ASwordWeapon::BeginPlay()
{
	Super::BeginPlay();
}
