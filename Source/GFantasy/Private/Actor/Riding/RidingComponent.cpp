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
	// 내리는 어빌리티 실행
	UnMount();
}

void URidingComponent::Mount()
{
	// 어빌리티로 탈 것 소환 후 Possess 하기 전, 플레이어를 캐싱
	CachedPlayer = Cast<AGASPlayerCharacter>(GetOwner());
	if (!CachedPlayer || !RidingActorClass)
		return;

	if (!GetOwner()->HasAuthority()) return;

	UWorld* World = CachedPlayer->GetWorld();
	if (!World)	return;
	
	APlayerController* PC = Cast<APlayerController>(CachedPlayer->GetController());
	if (!PC)	return;

	// 플레이어와의 충돌 비활성화
	UCapsuleComponent* Capsule = CachedPlayer->GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 플레이어 애니메이션 상태 변화 및 무브먼트 비활성화
	CachedPlayer->bIsMount = true;
	CachedPlayer->GetCharacterMovement()->DisableMovement();

	// 현재 플레이어를 탈 것 액터의 Rider로 설정
	SpawnedRidingActor->SetRider(CachedPlayer);

	// 캐릭터를 탈 것에 부착.
	CachedPlayer->AttachToComponent(
		SpawnedRidingActor->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FName("RidingSocket")
	);	

	// 쿨타임 부여
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
	
	// Possess 성공 후 → 명시적 IMC 적용
	if (AGASWorldPlayerController* PlayerPC = Cast<AGASWorldPlayerController>(CachedPlayer->GetController()))
	{
		PlayerPC->Client_ApplyIMCForCurrentPawn(true);
	}
}

// 탈 것 내리기
void URidingComponent::UnMount()
{
	// 서버에서 실행
	if (!SpawnedRidingActor || !CachedPlayer)
		return;

	APlayerController* PC = Cast<APlayerController>(SpawnedRidingActor->GetController());
	if (!PC)	return;

	// 캐릭터 탈착
	CachedPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 위치 조정
	//CachedPlayer->SetActorLocation(SpawnedRidingActor->GetActorLocation() + FVector(0, 0, 100.f));

	// 충돌 복원
	if (UCapsuleComponent* Capsule = CachedPlayer->GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// 이동 복원
	if (UCharacterMovementComponent* MoveComp = CachedPlayer->GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}
	// 탈 것 상태 변환.
	CachedPlayer->bIsMount = false;

	// 카메라 변경 후 Possess 변경
	PC->SetViewTargetWithBlend(CachedPlayer, 0.5f, VTBlend_Cubic, 2.0f); // 먼저 ViewTarget 변경

	// 쿨타임 부여
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

	// 현재 탈 것에 빙의를 해제하고 미리 캐싱해 둔 캐릭터에 다시 빙의.
	PC->UnPossess();
	PC->Possess(CachedPlayer);

	// 디졸브 연출로 탈 것 제거
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
		// MountCall 어빌리티 실행
		CachedPlayer->PlayerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(MountCallTag));
		UE_LOG(LogTemp, Warning, TEXT("Mount Call Play"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO PlayerASC"));
	}
}

// 탈 것 생성
void URidingComponent::Server_SpawnMountAtFront_Implementation()
{

	//디졸브 연출만 시작.
	if (!CachedPlayer)
	{
		CachedPlayer = Cast<AGASPlayerCharacter>(GetOwner());
	}
	
	if (!CachedPlayer || !RidingActorClass)
		return;

	// 캐릭터의 앞 방향으로 탈 것을 소환
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

	// 디졸브 연출만 시작
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


