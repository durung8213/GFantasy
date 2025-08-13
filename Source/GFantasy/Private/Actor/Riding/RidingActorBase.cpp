// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Riding/RidingActorBase.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Character/GASPlayerCharacter.h"
#include "Actor/Riding/RidingComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/InputSettings.h"
#include "InputAction.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Controller/GASWorldPlayerController.h"

// Sets default values
ARidingActorBase::ARidingActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// ���� Ȱ��ȭ
	bReplicates = true;
	SetReplicateMovement(true);

	// �̵� ����
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;

	// Controller Rotation�� ���� ������� ���� ( ���� ��� �̵� )
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;  
	
	RidingCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("RidingCamera"));
	RidingCamera->SetupAttachment(CameraBoom);
	RidingCamera->bUsePawnControlRotation = false;
	
	Armor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Armor"));
	Armor->SetupAttachment(GetMesh()); // ���ϴ� ���Ͽ� ���̱�
	
	Props = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Props"));
	Props->SetupAttachment(GetMesh()); // ���ϴ� ���Ͽ� ���̱�
	//
	// Horn = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Horn"));
	// Horn->SetupAttachment(GetMesh()); // ���ϴ� ���Ͽ� ���̱�
	//

}

void ARidingActorBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ARidingActorBase::Move);
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ARidingActorBase::Look);
		EIC->BindAction(IA_UnMount, ETriggerEvent::Triggered, this, &ARidingActorBase::OnXPressed);
	}
}

void ARidingActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void ARidingActorBase::Server_RequestUnMount_Implementation()
{
	if (Rider)
	{
		if (URidingComponent* Comp = Rider->FindComponentByClass<URidingComponent>())
		{
			Comp->UnMount(); // �������� ���� ����
		}
	}
}

void ARidingActorBase::Multicast_StartDissolveIn_Implementation()
{
	// ���׸��� ���� �ν��Ͻ� ȭ
	TArray<USkeletalMeshComponent*> MeshesToDissolve = { GetMesh(), Armor,  Props};

	// �� ���̷�Ż �޽��� ���׸����� ��� ���� ���׸����� �����ؼ� ����.
	for (USkeletalMeshComponent* MeshComp : MeshesToDissolve)
	{
		if (!MeshComp)	continue;
		for (int32 i = 0; i < MeshComp->GetNumMaterials();++i)
		{
			UMaterialInterface* BaseMat = MeshComp->GetMaterial(i);
			if (!BaseMat) continue;
		
			// �ش� ���� ���׸����� ���� �� ����
			UMaterialInstanceDynamic* DynMat = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
			if (DynMat)
			{
				// ������ ȿ���� �ִ� ���׸����� ���� ���� �����Ѵ�.
				DynMat->SetScalarParameterValue("DissolveAmount", 1.f);
				DissolveMats.Add(DynMat);
			}
		}
	}

	// ���� Ÿ�̸� ����
	FadeAlpha = 1.f;
	GetWorld()->GetTimerManager().SetTimer(FadeTimer, this, &ARidingActorBase::UpdateDissolveIn, 0.05f, true);
}

void ARidingActorBase::Multicast_StartDissolveOut_Implementation()
{
	// ���׸��� ���� �ν��Ͻ� ȭ
	TArray<USkeletalMeshComponent*> MeshesToDissolve = { GetMesh(), Armor,  Props};

	// 1. �� �޽� ������ �ʱ�ȭ.

	for (USkeletalMeshComponent* MeshComp : MeshesToDissolve)
	{
		if (!MeshComp)	continue;
		for (int32 i = 0; i < MeshComp->GetNumMaterials();++i)
		{
			UMaterialInterface* BaseMat = MeshComp->GetMaterial(i);
			if (!BaseMat) continue;
		
			UMaterialInstanceDynamic* DynMat = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
			if (DynMat)
			{
				DynMat->SetScalarParameterValue("DissolveAmount", 1.f);
				DissolveMats.Add(DynMat);
			}
		}
	}


	// ���� Ÿ�̸� ����
	FadeAlpha = 0.0f;
	GetWorld()->GetTimerManager().SetTimer(FadeTimer, this, &ARidingActorBase::UpdateDissolveOut, 0.05f, true);

}

void ARidingActorBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(NewController))
	{
		PC->SetViewTargetWithBlend(this, 0.8f);
		//Client_ApplyRidingIMC(); // Ŭ���̾�Ʈ ������ IMC ����
		PC->Client_ApplyIMCForCurrentPawn(true);
	}

	
}

void ARidingActorBase::Client_ApplyRidingIMC_Implementation()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, PC]()
		{
			if (const ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					Sub->ClearAllMappings();
					if (RidingIMC)
					{
						Sub->AddMappingContext(RidingIMC, 0);
						UE_LOG(LogTemp, Warning, TEXT("Riding IMC successfully applied."));
					}
				}
				else
				{
						UE_LOG(LogTemp, Warning, TEXT("No SUb : RidingActorBase"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No LP : RidingActorBase"));
			}
		});
	}
}

void ARidingActorBase::Move(const FInputActionValue& Value)
{

	FVector2D InputKey = Value.Get<FVector2D>();

	// �⺻ ĳ���� �̵�
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, InputKey.Y);
		AddMovementInput(RightDirection, InputKey.X);
	}
}

void ARidingActorBase::Look(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();

	AddControllerYawInput(Input.X * MouseSensitivity);
	AddControllerPitchInput(Input.Y * MouseSensitivity);
}


void ARidingActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARidingActorBase, Rider);
}

void ARidingActorBase::OnXPressed()
{
	bIsBeingUnmounted = true;
	if (!HasAuthority())
	{
		Server_RequestUnMount();
		return;
	}

	if (Rider)
	{
		
		if (URidingComponent* Comp = Rider->FindComponentByClass<URidingComponent>())
		{
			Comp->UnMount(); // ���� �Լ� ���� ȣ�� (�������� ���� ���̹Ƿ� ����)
		}
	}

}

void ARidingActorBase::OnNetCleanup(class UNetConnection* Connection)
{
	Super::OnNetCleanup(Connection);
	
	if (IsValid(Rider))
	{
		Rider->Destroy();
		Rider = nullptr;
	}
}

void ARidingActorBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// if (!bIsBeingUnmounted)
	// {
	// 	if (IsValid(Rider))
	// 	{
	// 		Rider->Destroy();
	// 		Rider = nullptr;
	// 	}
	// }
}

void ARidingActorBase::OnRep_Rider()
{

}

void ARidingActorBase::UpdateDissolveIn()
{
	FadeAlpha -= 0.1f;

	for (UMaterialInstanceDynamic* DynMat : DissolveMats)
	{
		DynMat->SetScalarParameterValue("DissolveAmount", FadeAlpha);
	}
	
	if (FadeAlpha <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimer);
		DissolveMats.Empty();
		//RiderDissolveMats.Empty();
	}
}

void ARidingActorBase::UpdateDissolveOut()
{
	FadeAlpha += 0.05f;

	for (UMaterialInstanceDynamic* DynMat : DissolveMats)
	{
		DynMat->SetScalarParameterValue("DissolveAmount", FadeAlpha);
	}
	
	if (FadeAlpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimer);
		DissolveMats.Empty();

		this->Destroy();
	}
}

void ARidingActorBase::SetMouseSensitivity(float NewSensitivity)
{
	MouseSensitivity = NewSensitivity;
}

float ARidingActorBase::GetMouseSensitivity() const
{
	return MouseSensitivity;
}

