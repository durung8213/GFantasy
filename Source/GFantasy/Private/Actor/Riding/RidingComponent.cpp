// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Riding/RidingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Character/GASPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Character/Weapon/WeaponBase.h"
#include "Controller/GASWorldPlayerController.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"

// Sets default values for this component's properties
URidingComponent::URidingComponent()
{
	SetIsReplicatedByDefault(true);
}

void URidingComponent::Server_Mount_Implementation()
{
	//Mount();
	MountCall();
}

void URidingComponent::Server_UnMount_Implementation()
{
	// ������ �����Ƽ ����
	UnMount();
}

void URidingComponent::Mount()
{
	// �����Ƽ�� Ż �� ��ȯ �� Possess �ϱ� ��, �÷��̾ ĳ��
	CachedPlayer = Cast<AGASPlayerCharacter>(GetOwner());
	if (!CachedPlayer || !RidingActorClass)
		return;

	if (!GetOwner()->HasAuthority()) return;

	UWorld* World = CachedPlayer->GetWorld();
	if (!World)	return;
	
	APlayerController* PC = Cast<APlayerController>(CachedPlayer->GetController());
	if (!PC)	return;

	// �÷��̾���� �浹 ��Ȱ��ȭ
	UCapsuleComponent* Capsule = CachedPlayer->GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// �÷��̾� �ִϸ��̼� ���� ��ȭ �� �����Ʈ ��Ȱ��ȭ
	CachedPlayer->bIsMount = true;
	CachedPlayer->GetCharacterMovement()->DisableMovement();

	// ���� �÷��̾ Ż �� ������ Rider�� ����
	SpawnedRidingActor->SetRider(CachedPlayer);

	// ĳ���͸� Ż �Ϳ� ����.
	CachedPlayer->AttachToComponent(
		SpawnedRidingActor->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FName("RidingSocket")
	);	

	// ��Ÿ�� �ο�
	if (MountCooldownEffect && CachedPlayer && CachedPlayer->PlayerASC)
	{
		FGameplayEffectContextHandle Context = CachedPlayer->PlayerASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = CachedPlayer->PlayerASC->MakeOutgoingSpec(MountCooldownEffect, 1.f, Context);
		if (SpecHandle.IsValid())
		{
			CachedPlayer->PlayerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			UE_LOG(LogTemp, Warning, TEXT("Cooldown Apply"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Spec Handle"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Effect Or No CachedPlayer"));
	}

	//PC->SetViewTargetWithBlend(SpawnedRidingActor, 0.5f, VTBlend_Cubic, 2.0f);

	PC->UnPossess();
	PC->Possess(SpawnedRidingActor);
	
	// Possess ���� �� �� ����� IMC ����
	if (AGASWorldPlayerController* PlayerPC = Cast<AGASWorldPlayerController>(CachedPlayer->GetController()))
	{
		PlayerPC->Client_ApplyIMCForCurrentPawn(true);
	}
}

// Ż �� ������
void URidingComponent::UnMount()
{
	// �������� ����
	if (!SpawnedRidingActor || !CachedPlayer)
		return;

	APlayerController* PC = Cast<APlayerController>(SpawnedRidingActor->GetController());
	if (!PC)	return;

	// ĳ���� Ż��
	CachedPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// ��ġ ����
	//CachedPlayer->SetActorLocation(SpawnedRidingActor->GetActorLocation() + FVector(0, 0, 100.f));

	// �浹 ����
	if (UCapsuleComponent* Capsule = CachedPlayer->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// �̵� ����
	if (UCharacterMovementComponent* MoveComp = CachedPlayer->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
	// Ż �� ���� ��ȯ.
	CachedPlayer->bIsMount = false;

	// ī�޶� ���� �� Possess ����
	PC->SetViewTargetWithBlend(CachedPlayer, 0.5f, VTBlend_Cubic, 2.0f); // ���� ViewTarget ����

	// ��Ÿ�� �ο�
	if (MountCooldownEffect && CachedPlayer && CachedPlayer->PlayerASC)
	{
		FGameplayEffectContextHandle Context = CachedPlayer->PlayerASC->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = CachedPlayer->PlayerASC->MakeOutgoingSpec(MountCooldownEffect, 1.f, Context);
		if (SpecHandle.IsValid())
		{
			CachedPlayer->PlayerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			UE_LOG(LogTemp, Warning, TEXT("Cooldown Apply"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Spec Handle"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Effect Or No CachedPlayer"));
	}

	// ���� Ż �Ϳ� ���Ǹ� �����ϰ� �̸� ĳ���� �� ĳ���Ϳ� �ٽ� ����.
	PC->UnPossess();
	PC->Possess(CachedPlayer);

	// ������ ����� Ż �� ����
	SpawnedRidingActor->Multicast_StartDissolveOut();
	SpawnedRidingActor = nullptr;
}

void URidingComponent::MountCall()
{
	FGameplayTag MountCallTag = FGameplayTag::RequestGameplayTag("Ability.Mount.MountCall");
	
	CachedPlayer = Cast<AGASPlayerCharacter>(GetOwner());
	if (!CachedPlayer)
		return;

	if (CachedPlayer->PlayerASC)
	{
		// MountCall �����Ƽ ����
		CachedPlayer->PlayerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(MountCallTag));
		UE_LOG(LogTemp, Warning, TEXT("Mount Call Play"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO PlayerASC"));
	}
}

// Ż �� ����
void URidingComponent::Server_SpawnMountAtFront_Implementation()
{

	//������ ���⸸ ����.
	if (!CachedPlayer)
	{
		CachedPlayer = Cast<AGASPlayerCharacter>(GetOwner());
	}
	
	if (!CachedPlayer || !RidingActorClass)
		return;

	// ĳ������ �� �������� Ż ���� ��ȯ
	FVector SpawnLocation = CachedPlayer->GetActorLocation() + FVector(0,0,15.f);
	FRotator SpawnRot = CachedPlayer->GetActorRotation();

	FActorSpawnParameters Params;
	Params.Owner = CachedPlayer;
	SpawnedRidingActor = GetWorld()->SpawnActor<ARidingActorBase>(
		RidingActorClass,
		SpawnLocation,
		SpawnRot,
		Params);

	if (!SpawnedRidingActor)
		return;

	// ������ ���⸸ ����
	SpawnedRidingActor->Multicast_StartDissolveIn();
	
}

void URidingComponent::Multicast_MountVFX_Implementation(FVector Location)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		PuffVFX,
		Location,
		FRotator::ZeroRotator,
		FVector(0.3f));
}


void URidingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URidingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URidingComponent, SpawnedRidingActor);
}


