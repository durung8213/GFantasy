// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/GASPlayerCharacter.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h" // ����׿� ��ü �׸���
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "GAS/GameplayAbility/GA_AttackCombo.h"
#include "GameplayTagContainer.h"
#include "Actor/Riding/RidingComponent.h"
#include "Animation/AnimInstance.h"
#include "Interface/InteractionInterface.h"
#include "Character/AnimInstance/GASPlayerAnimInstance.h"
#include "Character/SkillBar/SkillBarComponent.h"
#include "PlayerState/GASBasePlayerState.h"
#include "PlayerState/GASWorldPlayerState.h"
#include "Controller/GASPlayerController.h"
#include "Controller/GASWorldPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Character/Weapon/WeaponBase.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Player/PlayerNameWidget.h"



AGASPlayerCharacter::AGASPlayerCharacter()
{
	//������ �Ҵ�

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 150.f, 30.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	SkillBarComp = CreateDefaultSubobject<USkillBarComponent>("SkillBarComp");

	/* �г��� Text ������Ʈ */
	// �ؽ�Ʈ ������Ʈ ���� �� ��Ʈ�� ����
	NicknameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NicknameText"));
	NicknameText->SetupAttachment(RootComponent);
	// �ؽ�Ʈ ��ġ�� ĳ���� �Ӹ� ���� ����
	NicknameText->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	NicknameText->SetHorizontalAlignment(EHTA_Center);
	NicknameText->SetTextRenderColor(FColor::White);
	NicknameText->SetXScale(1.0f);
	NicknameText->SetYScale(1.0f);
	NicknameText->SetWorldSize(20.f);  // ���� ũ��
	NicknameText->SetText(FText::FromString(TEXT("Name")));

	// 2) WidgetComponent ���� (PlayerNameWidgetComponent)
	PlayerNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameWidget"));
	// ĳ���� �Ӹ� ���� ���� ���� ��� ��ġ
	PlayerNameWidgetComponent->SetupAttachment(RootComponent);
	PlayerNameWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	// ȭ�� �������� ���� �ʹٸ� EWidgetSpace::Screen
	PlayerNameWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// ���� ũ�⸦ ���� ���� �������� ���߱�
	//PlayerNameWidgetComponent->SetDrawAtDesiredSize(true);

	// ���̵� ������Ʈ ����
	RidingComponent = CreateDefaultSubobject<URidingComponent>(TEXT("RidingComponent"));
}

void AGASPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	static const TMap<FName, FString> DefaultMeshes = {
		{ "Hair", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ASTRA_HAIR_MID" },
		{ "Bangs", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ASTRA_HAIR_BANGS" },
		{ "Teeth", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_TEETH" },
		{ "Eyes", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_EYES" },
		{ "EyesLashes", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_LASHES" },
		{ "EyesCaruncle", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_CARUNCLE" },
		{ "Neck", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_BODY_PART" },
		{ "Skirt", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_CLOTH_with_bones" },
		{ "Head", "/Game/Asset/Character/IdaFaber/Meshes/Girl/Separated/SK_ROGUE_F_HEAD.SK_ROGUE_F_HEAD" }
	};
	
	for (const TPair<FName, FString>& Pair : DefaultMeshes)
	{
		const FName PartName = Pair.Key;
		const FSoftObjectPath AssetPath(Pair.Value);
		
		USkeletalMeshComponent* NewPart = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), PartName);
		NewPart->SetupAttachment(GetMesh());
		NewPart->RegisterComponent();
		//NewPart->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("root_socket"));
		//NewPart->SetLeaderPoseComponent(GetMesh(), true, true);
		//NewPart->SetLeaderPoseComponent(nullptr, false, false);
		NewPart->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		
		if (PartName == "Hair" || PartName == "Bangs")
		{
			NewPart->SetAnimInstanceClass(HairAnimInstanceClass);			
		}
		else
		{
			NewPart->SetAnimInstanceClass(MainAnimInstanceClass);
			NewPart->SetLeaderPoseComponent(GetMesh(), true, true);
		}

		USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *Pair.Value));
		if (LoadedMesh)
		{
			NewPart->SetSkeletalMesh(LoadedMesh);
		}
		
		MeshParts.Add(PartName, NewPart);

		if (NewPart->GetNumMaterials() > 0)
		{
			UMaterialInterface* BaseMaterial = NewPart->GetMaterial(0);
			if (BaseMaterial)
			{
				UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMaterial, this);
				NewPart->SetMaterial(0, MID);
				PartsMIDs.Add(PartName, MID);
			}
		}
	}
}

FString AGASPlayerCharacter::GetCharacterNickname()
{
	return CharacterNickname;
}

void AGASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();	

	// �׽�Ʈ�� �г��� ���� (���߿� �������� �޾ƿ��� �ɷ� ��ü)
	//SetNicknameText();
	SetNicknameInWidget(CharacterNickname);
}

// ĳ������ ���� ����
void AGASPlayerCharacter::Server_PerformAutoTargeting_Implementation()
{

	if (TargetingState == ETargetingState::LockOn)
		return;		// ���� �߿� ���ο� ��Ŀ�� ����

	if (bIsMount)
	{
		return;
	}
	// Overlap ����
	float Radius = 1000.0f;
	// �þ߰�
	float FOV = 90.0f;
	FVector MyLocation = GetActorLocation();


	TArray<AActor*> HitActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// �÷��̾� �ڽ��� �������� �ʵ��� ����
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	// World�� ��ȿ���� Ȯ��
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("PossessedBy: GetWorld() returned null!"));
		return;
	}

	// SphereOverlapActors ����
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		Radius,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		HitActors
	);


	// ������ ���� ���� Target �迭�� �ڵ� ��Ŀ�̵� BestTarget ����
	AActor* BestTarget = nullptr;
	float BestDistSq = FLT_MAX;
	Targets.Empty();

	if (bHit)
	{
		for (AActor* Result : HitActors)
		{
			// �������� ���Ͱ� ������ Ȯ��
			if (!Result || !Result->ActorHasTag("Enemy"))
				continue;

			// �������� ���� �÷��̾� �þ߿� �ִ��� Ȯ��
			if (!IsInCameraFOV(Result, FOV))
			{
				UKismetSystemLibrary::PrintString(this, TEXT("FOV FALSE"));
				continue;
			}

			float DistSq = FVector::DistSquared(MyLocation, Result->GetActorLocation());

			// ���� �����̿� �ִ� ���� ã��.
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				Targets.AddUnique(Result);
				BestTarget = Result;
			}
		}
	}
	else
	{
		//�������� ���� ���ٸ� Ÿ���� ���¸� None ���� �ʱ�ȭ.
		TargetingState = ETargetingState::None;
	}

	// ���ŵ� Ÿ���� ���� �� Ÿ���� �ƴ϶�� ����
 	if (BestTarget != CurrentTarget)
	{
		// ���� Ÿ�� �缳��
		CurrentTarget = BestTarget;
		if (CurrentTarget)
		{
			TargetingState = ETargetingState::Focusing;
			UE_LOG(LogTemp, Warning, TEXT("TARGET DETECT : %s"), *CurrentTarget->GetName());
		}

		// ������ ���� ���� ���� ȣ�� �߰�
		EvaluateCombatState();   // bShouldBeInCombat ���
		UpdateCombatState();     // Ÿ�̸� ���� �� SetCombatState ȣ��
	}

}

void AGASPlayerCharacter::Server_TryLockOn_Implementation()
{
	HandleLockOnToggle();
}

void AGASPlayerCharacter::Server_TryInteract_Implementation()
{
	if (bIsMount)
	{
		return;
	}
	
	PC = Cast<AGASWorldPlayerController>(GetController());

	AActor* BestTarget = nullptr;
	float BestDistSq = FLT_MAX;

	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Controller found"));
		return;
	}

	FVector ViewLoc;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewLoc, ViewRot);
	FVector ViewDir = ViewRot.Vector();

	//�ش� �迭�� 0�� �ƴ� ��, �����ϵ���.
	if (!OverlappedInteractables.IsEmpty())
	{
		for (AActor* Actor : OverlappedInteractables)
		{
			if (!IsValid(Actor))
				continue;

			FVector ToTarget = (Actor->GetActorLocation() - ViewLoc).GetSafeNormal();
			float Dot = FVector::DotProduct(ViewDir, ToTarget);
			float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(Dot));

			if (AngleDeg > (120.0f * 0.5f))
				continue;

			float DistSq = FVector::DistSquared(Actor->GetActorLocation(), ViewLoc);
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestTarget = Actor;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OverlappedInteractables is Empty"));
	}

	// Interact ����
	if (BestTarget && BestTarget->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		// �ش� ���Ϳ� Interaction ��� ����
		IInteractionInterface::Execute_Interact(BestTarget, this);
	}

}

void AGASPlayerCharacter::Server_SetInputDirection_Implementation(FVector Direction)
{
	CachedInputDirection = Direction;
}


// Upper �ִϸ��̼� ��Ƽ�� �ѷ��ֱ�.
void AGASPlayerCharacter::Multicast_PlayUpperBodyMontage_Implementation(
	UAnimMontage* Montage, bool bBlendOn)
{
	bHasUpperBodyAction = bBlendOn;
	OnRep_UpperBodyBlend();  // Ŭ�󿡼��� �ٷ� ����

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		if (bBlendOn && Montage)
		{
			// ��Ÿ�� ��� (�ߺ� ����)
			if (!Anim->Montage_IsPlaying(Montage))
				Anim->Montage_Play(Montage, 1.0f);
		}
		else if (!bBlendOn && Montage)
		{
			// �ִϸ��̼� ����
			Anim->Montage_Stop(0.25f, Montage);
		}
	}
}

void AGASPlayerCharacter::Server_TryActivateLimitBreak_Implementation()
{
	TryActivateLimitBreak();
}

void AGASPlayerCharacter::Server_StartSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 850.0f;

	if (UGASPlayerAnimInstance* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsSprint = true;
	}
}

void AGASPlayerCharacter::Server_StopSprint_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	if (UGASPlayerAnimInstance* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsSprint = false;
	}
}

void AGASPlayerCharacter::Multicast_AttachWeaponToSocket_Implementation(FName SocketName) const
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketName);
	}
}

// ���� Ÿ�� ����	�� ���� ���� ������Ʈ
void AGASPlayerCharacter::OnRep_CurrentTarget()
{
	if (IsValid(CurrentTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("TARGET DETECT : %s"), *CurrentTarget->GetName());
	}
	
	// ���� / ������ ���� üũ ����
	EvaluateCombatState();	// ���� ���� ���� ���� üũ
	UpdateCombatState();	// Ÿ�� ������ Ÿ�̸� ����.
}

void AGASPlayerCharacter::OnRep_TargetingState()
{
	switch (TargetingState)
	{
	case ETargetingState::None:
		break;
	case ETargetingState::LockOn:
		break;
	case ETargetingState::Focusing:
		break;
	default:
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("TargetingState : %d"), TargetingState);
}


void AGASPlayerCharacter::OnRep_UpperBodyBlend()
{
	if (auto* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bHasUpperBodyAction = bHasUpperBodyAction;
	}
}

void AGASPlayerCharacter::OnRep_IsDead()
{
	if (auto* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsDead = bIsDead;
	}
}

void AGASPlayerCharacter::OnRep_bIsRespawned()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (UGASPlayerAnimInstance* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			Anim->bIsRespawned = bIsRespawned;
		}
	}, 0.1f, false);
}

void AGASPlayerCharacter::OnRep_IsUIShow()
{
	if (bIsUIShow)
	{
		// ���� IMC �����ϱ�1
		ApplyIMC(IMC_Interaction, 0);
		UE_LOG(LogTemp, Warning, TEXT("UI Open Mode"));
	}
	else
	{
		if (CurrentWeapon)
		{
			ApplyIMC(IMC_SwordArmed, 0);
			UE_LOG(LogTemp, Warning, TEXT("UI Close : Weapon Mode"));
		}
		else
		{
			ApplyIMC(IMC_UnArmed, 0);
			UE_LOG(LogTemp, Warning, TEXT("UI Close : UnArmed Mode"));
		}
	}
}

void AGASPlayerCharacter::OnRep_IsMount()
{
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		if (UGASPlayerAnimInstance* PlayerAnim = Cast<UGASPlayerAnimInstance>(Anim))
		{
			PlayerAnim->bIsMounted = bIsMount;
		}
	}
}

void AGASPlayerCharacter::OnRep_CombatState()
{
	
	if (auto* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsInCombat = (CombatState == ECombatState::InCombat);
	}
}

void AGASPlayerCharacter::OnRep_CurrentWeapon()
{
	EvaluateCombatState();

	if (CurrentWeapon)
	{
		if (auto* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			Anim->bHasWeapon = true;
		}
	}
	else if (CurrentWeapon == nullptr)
	{
		if (auto* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			Anim->bHasWeapon = false;
		}
	}
}

bool AGASPlayerCharacter::IsInCameraFOV(AActor* TargetActor, float FOVDegrees) const
{
	if (!TargetActor || !FollowCamera)
		return false;

	// ī�޶󿡼� Ÿ������ ���ϴ� ���͸� ����
	FVector ToTarget = (TargetActor->GetActorLocation() - FollowCamera->GetComponentLocation()).GetSafeNormal();

	// ī�޶��� �� ���� ���͸� ����
	FVector CameraForward = FollowCamera->GetForwardVector();

	// �� ������ ������ ����                                                                                                                                                   
	float DotProduct = FVector::DotProduct(CameraForward, ToTarget);

	// �� ������ ������ ����.
	float Radians = FMath::Acos(DotProduct);

	float Degrees = FMath::RadiansToDegrees(Radians);

	//���� ������ �þ߰� ���̸� true�� ��ȯ
	return Degrees <= (FOVDegrees * 0.5f);	// ���� �þ߰� (�¿�) ��ȯ
}

void AGASPlayerCharacter::TryLockOn()
{
	if (bIsDead) return;

	if (HasAuthority())
	{
		HandleLockOnToggle();
	}
	else
	{
		Server_TryLockOn();
	}
}

void AGASPlayerCharacter::ReleaseLockOn()
{
	//���� ���� ���¶��
	TargetingState = ETargetingState::Focusing;

	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	CameraBoom->SocketOffset = FVector(0.f, 150.f, 30.f);

	// ���� ����
	bIsLockOn = false;
}

void AGASPlayerCharacter::DodgeAction()
{
	if (!PlayerASC)
		return;
	
	const FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat");
	if (PlayerASC->HasMatchingGameplayTag(BlockTag))
	{
		const FGameplayTag PatternTag = FGameplayTag::RequestGameplayTag("Combat.Pattern1");
		if (PlayerASC->HasMatchingGameplayTag(PatternTag))
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FGameplayTag::RequestGameplayTag("Combat.Pattern1"));
			PlayerASC->TryActivateAbilitiesByTag(TagContainer);
		}
		
		UKismetSystemLibrary::PrintString(this, TEXT("Ability Blocked"));
		return;
	}

	
	// �����Ƽ ����
	PlayerASC->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Dodge));

}

void AGASPlayerCharacter::HandleHitReact(AActor* HitInstigator, float DamageAmount, bool bStrong)
{
	// ������ ��ȿ�� Ÿ���� ���� ��, �� �Լ��� �����. 
	UE_LOG(LogTemp, Warning, TEXT("HandleHit"));
	// 1. ���� ���
	FVector Dir = (HitInstigator->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	FVector Forward = GetActorForwardVector().GetSafeNormal2D();

	float Dot = FVector::DotProduct(Forward, Dir);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
	FVector Cross = FVector::CrossProduct(Forward, Dir);

	// 2. �±� ����

	FGameplayTag ReactTag;

	if (!bStrong)
	{
		if (Angle <= 45.f)
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Front");
		else if (Angle > 135.f)
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Back");
		else if (Cross.Z > 0)
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Right");
		else
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Left");
	}
	else
	{
		if (Angle <= 45.f)
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Strong.Front");
		else if (Angle > 135.f)
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Strong.Back");
		else if (Cross.Z > 0)
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Strong.Right");
		else
			ReactTag = FGameplayTag::RequestGameplayTag("Ability.HitReact.Strong.Left");
	}


	// 3. �����Ƽ ����
	UE_LOG(LogTemp, Warning, TEXT("GE Ability Start"));

	if (PlayerASC)
	{
		PlayerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ReactTag));
	}

}


void AGASPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Ÿ���� �����ϰ�, LockOn ���¶��
	if (TargetingState == ETargetingState::LockOn && CurrentTarget)
	{
		PC = Cast<AGASWorldPlayerController>(GetController());
		if (!PC)
		{
			UE_LOG(LogTemp, Warning, TEXT("No PC"));
			return;
		}

		// ���� Ÿ���� �׾��� ��, �ش� Tag�� ������ �����ϵ���
		if (CurrentTarget->IsActorBeingDestroyed() || !IsValid(CurrentTarget) ||
			!CurrentTarget->ActorHasTag("Enemy"))
		{
			// ���� ����
			ReleaseLockOn();
			return;
		}

		float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

		if (Distance > 1500.f)
		{
			// ���� �� ������ �Ÿ��� �ָ� Target ����
			ReleaseLockOn();
			return;
		}

		// �� ��ġ ����, Ÿ�� ��ġ ���� ���
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = CurrentTarget->GetActorLocation();

		FRotator DesiredRot = (TargetLoc - MyLoc).Rotation();
		DesiredRot.Pitch = -7.f;
		DesiredRot.Roll = 0.f;

		FRotator CurrentRot = PC->GetControlRotation();
		//���� ȸ��
		FRotator InterpRot = FMath::RInterpTo(CurrentRot, DesiredRot, DeltaTime, CameraRotationInterpSpeed);

		// ī�޶� ȸ��
		PC->SetControlRotation(InterpRot);
	}
	else
	{
	}

	// �г����� �׻� ī�޶� �ٶ󺸵���
	if (NicknameText)
	{
		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (CameraManager)
		{
			FVector CameraLocation = CameraManager->GetCameraLocation();
			FVector LookAtDirection = CameraLocation - NicknameText->GetComponentLocation();
			FRotator NewRotation = FRotationMatrix::MakeFromX(LookAtDirection).Rotator();
			NicknameText->SetWorldRotation(NewRotation);
		}
	}
}

void AGASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PC = Cast<AGASWorldPlayerController>(GetController());

	if (PC)
	{
		// IMC �����ϰ� ĳ���ϱ�.
		ApplyIMC(IMC_UnArmed, 0);
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AGASPlayerCharacter::Move);
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AGASPlayerCharacter::Look);

		EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnAttackPressed);
		EIC->BindAction(IA_Attack, ETriggerEvent::Completed, this, &AGASPlayerCharacter::OnAttackReleased);

		EIC->BindAction(IA_LockOn, ETriggerEvent::Started, this, &AGASPlayerCharacter::TryLockOn);

		EIC->BindAction(IA_RangedAttack, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnRangedAttackPressed);
		EIC->BindAction(IA_Dodge, ETriggerEvent::Started, this, &AGASPlayerCharacter::DodgeAction);
		EIC->BindAction(IA_Interaction, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnInteractPressed);

		EIC->BindAction(IA_Q, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnQPressed);
		EIC->BindAction(IA_E, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnEPressed);
		EIC->BindAction(IA_X, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnXPressed);
		EIC->BindAction(IA_T, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnTPressed);
		EIC->BindAction(IA_Dash, ETriggerEvent::Started, this, &AGASPlayerCharacter::OnSprintStarted);
		EIC->BindAction(IA_Dash, ETriggerEvent::Completed, this, &AGASPlayerCharacter::OnSprintFinished);
	}
}

void AGASPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PC = Cast<AGASWorldPlayerController>(NewController);	

	
	// ���������� ASC �ʱ�ȭ �� �ɷ�ġ �ο�
	if (HasAuthority())
	{
		bIsMount = false;

		// Ÿ������ ã�� ���� Ÿ�̸� ����
		UE_LOG(LogTemp, Warning, TEXT("Interaction Detect Start"));
		GetWorldTimerManager().SetTimer(
			TargetingTH,
			this,
			&AGASPlayerCharacter::Server_PerformAutoTargeting,
			0.25f,
			true
		);
		
		// ��ȣ�ۿ��� ���� Ÿ�̸� ����
		UE_LOG(LogTemp, Warning, TEXT("DETECT START"));
		GetWorldTimerManager().SetTimer(
			InteractionTH,
			this,
			&AGASPlayerCharacter::DetectInteractableUI,
			0.3f,	// �� 3�ʸ��� �˻�
			true
		);



		AGASBasePlayerState* PS = GetPlayerState<AGASBasePlayerState>();
		if (PS && PS->GetAbilitySystemComponent())
		{
			PlayerASC = Cast<UPlayerCharacterGASC>(PS->GetAbilitySystemComponent());
			PlayerASC->InitAbilityActorInfo(PS, this);

			// �� �ѹ��� ����ǵ��� ����.
			if (!PS->bIsInitialize)
			{
				PS->bIsInitialize = true;

				//ASC �ʱ�ȭ �Ϸ� �ɷ�/ ȿ�� ����
				PS->InitDefaultAttributes();


				if (PS->CharacterAS)
				{
					PS->CharacterAS->OnDamageDelegate.AddDynamic(this, &AGASPlayerCharacter::HandleHitReact);
					PS->CharacterAS->IncreaseLimitGage.AddDynamic(PlayerASC, &UPlayerCharacterGASC::HandleLimitGageEvent);
				}
			}
		}

		if (PlayerAbilitySet && HasAuthority())
		{
			// DataSet�� ������ �����Ƽ ��� �ο�.
			PlayerAbilitySet->GiveAbilitiesToASC(PlayerASC);
		}

		if (PC && SkillBarComp)
		{
			if (!SkillBarComp->bIsInitialized)
			{
				SkillBarComp->InitializeSkillBar();
				SkillBarComp->bIsInitialized = true;
				UE_LOG(LogTemp, Warning, TEXT("SkillBar Initialized in PossessedBy"));
			}
		}

		FTimerHandle DelayHandle;
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			UpdateCombatState();
			SetCombatState(GetCombatState());
		});

		if (bIsRespawned)
		{
			FTimerHandle RespawnHandle;
			GetWorld()->GetTimerManager().SetTimer(RespawnHandle, [this]()
			{
				if (PlayerASC && PlayerASC->GetAvatarActor() == this)
				{
					FGameplayTag GetUpTag = FGameplayTag::RequestGameplayTag("Ability.GetUp");
					PlayerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(GetUpTag));
					bIsRespawned = false;	// �ڵ� Idle ��ȯ�� ���� ���� �ʱ�ȭ
				}
			}, 0.1f, false);
		}
	}	

	if (PC)
	{
		PC->UpdateIMCFromCurrentWeaponSlot();
		UE_LOG(LogTemp, Warning, TEXT("IMC Update in PossessedBy"));
		//PC->SetViewTargetWithBlend(this, 0.5f, VTBlend_Cubic, 2.0f);
	}


}

void AGASPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Ŀ���͸����� ������Ʈ ��������Ʈ ����
	BindCallbacksToDependencies();

	if (AGASWorldPlayerState* PS = GetPlayerState<AGASWorldPlayerState>())
	{
		UCustomizerComponent* CustomizerComponent = ICustomizerInterface::Execute_GetCustomizerComponent(PS);
		for (const FGFCustomizerEntry& Entry : CustomizerComponent->GetCustomizerEntries())
		{
			SetCustomizeMesh(Entry);
		}
	}	
	
	// PlayerState�� ���� ���� �� ����, InitAbilityActorInfo�� �������ش�.

	UE_LOG(LogTemp, Log, TEXT("OnRep_PlayerState called"));

	// PlayerState�� ���� ���� �� ����, InitAbilityActorInfo�� �������ش�.
	AGASBasePlayerState* PS = GetPlayerState<AGASBasePlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState: Failed to get AGASBasePlayerState!"));
		return;
	}

	UAbilitySystemComponent* AbilitySystemComp = PS->GetAbilitySystemComponent();
	if (!AbilitySystemComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState: AbilitySystemComponent is null!"));
		return;
	}

	PlayerASC = Cast<UPlayerCharacterGASC>(AbilitySystemComp);
	if (!PlayerASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerState: Failed to cast ASC to UPlayerCharacterGASC!"));
		return;
	}

	if (PS)
	{
		// Ŭ���̾�Ʈ �� �ʱ�ȭ.
		UE_LOG(LogTemp, Log, TEXT("OnRep_PlayerState: Initializing ASC ActorInfo..."));
		PlayerASC->InitAbilityActorInfo(PS, this);
	}
	PC = Cast<AGASWorldPlayerController>(GetController());
	if (PC && PC->IsLocalController())
	{
		PC->SetViewTargetWithBlend(this, 0.5f);

		AActor* ViewTarget = PC->GetViewTarget();
		UE_LOG(LogTemp, Warning, TEXT("Current ViewTarget is: %s"), *GetNameSafe(ViewTarget));
	}
}


void AGASPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bIsDead)	return;

	FVector2D InputKey = Value.Get<FVector2D>();

	// �⺻ ĳ���� �̵�
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		FVector MoveDir = (ForwardDirection * InputKey.Y) + (RightDirection * InputKey.X);
		
		if (!MoveDir.IsNearlyZero())
		{
			CachedInputDirection = MoveDir.GetSafeNormal();
			Server_SetInputDirection(CachedInputDirection);
			AddMovementInput(MoveDir);
		}
	}
}

void AGASPlayerCharacter::Look(const FInputActionValue& Value)
{
	if (bIsDead)	return;

	FVector2D v = Value.Get<FVector2D>();

	if (TargetingState == ETargetingState::LockOn)
	{
		// ���� �߿��� ī�޶� �������� ���ϵ��� ����.
		return;
	}


	if (Controller != nullptr)
	{
		AddControllerYawInput(v.X * MouseSensitivity);
		AddControllerPitchInput(v.Y * MouseSensitivity);
	}
}

// ���콺 ��Ŭ��
// ����Ű�� ������ �Ǹ�
void AGASPlayerCharacter::OnAttackPressed()
{
	if (bIsUIShow)
	{
		UE_LOG(LogTemp,Log,TEXT("Player In UI Mode"));
		return;
	}
	
	if (!PlayerASC)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("NO PlayerASC"));
		return;
	}

	const FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat");
	const FGameplayTag ComboTag = FGameplayTag::RequestGameplayTag("Ability.Attack.Combo");

	// �ٸ� �����Ƽ�� �������� ��,
	if (PlayerASC->HasMatchingGameplayTag(BlockTag))
	{
		// �ش� �����Ƽ�� �޺� �±׵� ���ٸ� return
		if (!PlayerASC->HasMatchingGameplayTag(ComboTag))
		{
			UKismetSystemLibrary::PrintString(this, TEXT("Combo Blocked"));
			return;
		}
	}

	// �����Ƽ�� ����Ǿ� ���� �ʴٸ� �����Ƽ�� �����ϰ� InputPressed�� ����
	PlayerASC->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Attack));

}

void AGASPlayerCharacter::OnAttackReleased()
{
	if (PlayerASC)
	{
		PlayerASC->AbilityLocalInputReleased(static_cast<int32>(EAbilityInputID::Attack));
	}
}

void AGASPlayerCharacter::OnRangedAttackPressed()
{
	if (!PlayerASC)
		return;

	const FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat");
	if (PlayerASC->HasMatchingGameplayTag(BlockTag))
	{
		UKismetSystemLibrary::PrintString(this, TEXT("Ability Blocked"));
		return;
	}

	PlayerASC->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::RangedAttack));
}

// R ��ư ���� ��, ���� ��� ����Ī
void AGASPlayerCharacter::HandleLockOnToggle()
{
	// �̹� ���� ����� ����
	if (TargetingState == ETargetingState::LockOn)
	{
		ReleaseLockOn();
		return;
	}

	// ���� ��Ŀ�̻����̰�, Ÿ���� ��ȿ�� �� ���� ��� ����
	if (TargetingState == ETargetingState::Focusing && IsValid(CurrentTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("In Lock On Mode"));
		// ���� ��� ����

		// ī�޶� ��ġ ���
		CameraBoom->TargetArmLength = 500.f;
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 200.f);


		// ���� ����
		TargetingState = ETargetingState::LockOn;
		bIsLockOn = true;

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Lock On Fail"));
		UE_LOG(LogTemp, Warning, TEXT("%d"), TargetingState);
	}
}

void AGASPlayerCharacter::OnInteractPressed()
{
	// F Ű ���� �� Ȱ��ȭ.
	Server_TryInteract();
}

void AGASPlayerCharacter::OnQPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("On Q Pressed"));
	// Q ��ư ���� ��, ��ų ����

	if (CurrentTarget != nullptr)
	{
		SkillBarComp->ActivateSlot(EAbilitySlot::SlotQ);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Q Skill UnUse : No Current Target"));
}

void AGASPlayerCharacter::OnEPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("On E Pressed"));
	// E ��ư ���� ��, ��ų ����
	SkillBarComp->ActivateSlot(EAbilitySlot::SlotE);
}

void AGASPlayerCharacter::OnXPressed()
{

	UE_LOG(LogTemp, Warning, TEXT("On X Pressed"));

	if (!RidingComponent) return;

	if (!PlayerASC)
		return;

	if (PlayerASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat")))
	{
		
		UE_LOG(LogTemp, Warning, TEXT("On X Combat"));
		return;
	}
	
	if (PlayerASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("CoolDown.Interaction.Riding")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mount Cooldown still active!"));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Cooldown"));
	}

	PC = Cast<AGASWorldPlayerController>(GetController());
	if (!PC)	return;
	

	if (RidingComponent->IsMounted())
	{
		RidingComponent->Server_UnMount();
	}
	else
	{
		bIsMount = true;
		ApplyIMC(IMC_Interaction,0);
		RidingComponent->Server_Mount();
	}

}

void AGASPlayerCharacter::OnTPressed()
{
	if (!PlayerASC)
		return;

	const FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag("State.Ability.Blocked.Combat");

	if (IsValid(PlayerASC))
	{
		if (HasAuthority())
		{
			TryActivateLimitBreak();
		}
		else
		{
			Server_TryActivateLimitBreak();
		}
	}
}

// �뽬 ����
void AGASPlayerCharacter::OnSprintStarted()
{
	GetCharacterMovement()->MaxWalkSpeed = 850.0f;
	
	if (UGASPlayerAnimInstance* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsSprint = true;
	}
	if (!HasAuthority())
	{
		Server_StartSprint();
	}
}

void AGASPlayerCharacter::OnSprintFinished()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	if (UGASPlayerAnimInstance* Anim = Cast<UGASPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->bIsSprint = true;
	}
	
	if (!HasAuthority())
	{
		Server_StopSprint();
	}
}

void AGASPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGASPlayerCharacter, TargetingState);
	DOREPLIFETIME(AGASPlayerCharacter, bIsLockOn);
	DOREPLIFETIME(AGASPlayerCharacter, CurrentTarget);
	DOREPLIFETIME(AGASPlayerCharacter, CachedInputDirection);
	DOREPLIFETIME(AGASPlayerCharacter, CharacterNickname);
	DOREPLIFETIME(AGASPlayerCharacter, bHasUpperBodyAction);
	DOREPLIFETIME(AGASPlayerCharacter, bIsDead);
	DOREPLIFETIME(AGASPlayerCharacter, bIsMount);
	DOREPLIFETIME(AGASPlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(AGASPlayerCharacter, bIsUIShow);
	DOREPLIFETIME(AGASPlayerCharacter, CombatState);
	DOREPLIFETIME(AGASPlayerCharacter, bIsRespawned);
}

void AGASPlayerCharacter::DetectInteractableUI()
{
	TArray<AActor*> HitActors;
	TArray<AActor*> Ignore;
	Ignore.Add(this);

	// World�� ��ȿ���� Ȯ��
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("PossessedBy: GetWorld() returned null!"));
		return;
	}

	// ��ȣ�ۿ��� ���� ������ ����
	UKismetSystemLibrary::SphereOverlapActors(
		World,
		GetActorLocation(),
		500.f,
		{ UEngineTypes::ConvertToObjectType(ECC_Pawn) },
		nullptr,
		Ignore,
		HitActors
	);

	// ���� ���� ���
	OverlappedInteractables.Empty();

	for (AActor* Actor : HitActors)
	{
		if (!Actor->ActorHasTag(FName("Interactable")))
			continue;

		OverlappedInteractables.AddUnique(Actor);

		// �� ��� ���� UI ǥ��
		// ���� ���
		// ��ȣ�ۿ밡���� �͵鿡�Դ� ��� ������ �Լ��� �ʿ�.
		//CreateOrUpdateFocusingUI();
	}

}

void AGASPlayerCharacter::TryActivateLimitBreak()
{
	if (PlayerASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Limit.Ready")))
	{
		PlayerASC->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::LimitBreak));
		UE_LOG(LogTemp, Error, TEXT("Tag Ability Success : LimitBreak "));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Tag : LimitBreak "));
	}
}

// IMC �����ϱ�
void AGASPlayerCharacter::ApplyIMC(UInputMappingContext* NewIMC, int32 Priority)
{
	if (UEnhancedInputLocalPlayerSubsystem* Sub = GetEISubSystem())
	{

		UE_LOG(LogTemp, Warning, TEXT("Apply SubSystem"));
		Sub->ClearAllMappings();
		Sub->AddMappingContext(NewIMC, Priority);
		CachedIMC = NewIMC;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No SubSystem"));
	}
}

// EnhancedInput SubSystem ��������.
UEnhancedInputLocalPlayerSubsystem* AGASPlayerCharacter::GetEISubSystem()
{
	PC = Cast<AGASWorldPlayerController>(GetController());
	if (!PC)		return nullptr;

	if (const ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		UE_LOG(LogTemp, Warning, TEXT("Success EI SubSystem"));
		return ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);
	}

	UE_LOG(LogTemp, Warning, TEXT("No LP SubSystem"));
	return nullptr;
}

// ���� ���� ���������� üũ.
void AGASPlayerCharacter::UpdateCombatState()
{
	// ���� ���¶�� üũ�� �ʿ� X
	if (CombatState != ECombatState::InCombat)
		return;

	if (CurrentTarget == nullptr)
	{	
		//���� Ÿ���� ����, ���� ���� ���¸� üũ�ϴ� Ÿ�̸Ӱ� ������� �ʰ� �ִٸ� ����.
		if (!GetWorld()->GetTimerManager().IsTimerActive(CombatStateTimer))
		{
			//
			GetWorldTimerManager().SetTimer(
				CombatStateTimer,
				this,
				&AGASPlayerCharacter::HandleCombatStateTimeOut,
				CombatStateCooldownTime,
				false);
			
			UE_LOG(LogTemp, Warning, TEXT("8 Seconds Later In NonCombat Mode "));
		}
	}
	else
	{
		// Ÿ���� �ٽ� ����� Ÿ�̸� ����
		if (GetWorld()->GetTimerManager().IsTimerActive(CombatStateTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(CombatStateTimer);
			UE_LOG(LogTemp, Warning, TEXT("Timer Cancelled : Target is Not NULL"));
		}
	}
}

void AGASPlayerCharacter::SetCombatState(ECombatState NewState)
{
	if (CombatState == NewState)
	{
		return;
	}

	CombatState = NewState;
	OnRep_CombatState();
	// ���⼭ �����Ƽ ����
			
	// �ִϸ��̼� �����Ƽ ����
	if (PlayerASC)
	{
		const FGameplayTag EquipAbilityTag = FGameplayTag::RequestGameplayTag("Ability.EquipAnimation");
	
		PlayerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(EquipAbilityTag));
	}

	PC = Cast<AGASWorldPlayerController>(GetController());
	if (!PC)	return;
	
	if (PC)
	{
		// IMC Refresh
		PC->Client_RefreshInputContextFromCombatState(CombatState == ECombatState::InCombat);
	}
}

// Ÿ�̸� �ð� ���� Ȱ��ȭ : ������ ��� ��ȯ �Լ�
void AGASPlayerCharacter::HandleCombatStateTimeOut()
{
	if (CombatState == ECombatState::InCombat && CurrentTarget == nullptr)
	{
		SetCombatState(ECombatState::NonCombat);
		//ApplyIMC(IMC_UnArmed,0);
		UE_LOG(LogTemp,Warning,TEXT("Current CombatState : NON COMBAT")); 
	}
}

// ���� ���� üũ �Լ�
void AGASPlayerCharacter::EvaluateCombatState()
{
	// ���� ���ο� Target ���η� ���� ��� �Ǵ�
	const bool bHasWeapon = (CurrentWeapon != nullptr);
	const bool bHasTarget = (CurrentTarget != nullptr);
	const bool bShouldBeInCombat = bHasWeapon && bHasTarget;

	// ���� Combat �������� �ƴ����� �Ǵ�
	const ECombatState NewState = bShouldBeInCombat?ECombatState::InCombat : ECombatState::NonCombat;

	if (bShouldBeInCombat)
	{
		// ���� ����ε�, ������尡 �Ǿ����� �ʴٸ�
		if (CombatState != ECombatState::InCombat)
		{
			// ���� ���� ��ȯ.
			SetCombatState(ECombatState::InCombat);
			UE_LOG(LogTemp,Warning,TEXT("Combat State IMC Apply"));
		}

		// ���� ��� �� ��, Ÿ���� ���� ���� �׻� ������ ���� Ÿ�̸� �ʱ�ȭ
		GetWorld()->GetTimerManager().ClearTimer(CombatStateTimer);
	}
	
}

void AGASPlayerCharacter::AttachWeaponToSocket(FName SocketName) const
{
	if (CurrentWeapon)
	{
		// 1. ���� ����
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. �̸� ���� ��ġ�� ���� �̵�
		FTransform SocketTransform = GetMesh()->GetSocketTransform(SocketName);
		CurrentWeapon->SetActorTransform(SocketTransform);

		// 3. ���Ͽ� ���̱�
		CurrentWeapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			SocketName);
		UE_LOG(LogTemp,Warning, TEXT("Attach Success"));
	}
	else
	{
		UE_LOG(LogTemp,Warning, TEXT("In AttachWeaponToSocket : No CurrentWeapon"));
	}
}


void AGASPlayerCharacter::OnNetCleanup(class UNetConnection* Connection)
{
	Super::OnNetCleanup(Connection);

	UE_LOG(LogTemp, Warning, TEXT("OnNetCleanup called."));


	
}

void AGASPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// ���Ⱑ �ִٸ� ����
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
}

void AGASPlayerCharacter::SetCustomizeMesh(const FGFCustomizerEntry& CustomizerEntry)
{	
	if (CustomizerEntry.SkeletalMeshName == FName(TEXT("Hair")))
	{
		if (USkeletalMeshComponent** FoundPart = MeshParts.Find(CustomizerEntry.SkeletalMeshName))
		{			
			if (FoundPart && *FoundPart && CustomizerEntry.CustomMesh)
			{				
				USkeletalMesh* LoadedMesh = CustomizerEntry.CustomMesh;				
				(*FoundPart)->SetSkeletalMesh(LoadedMesh);

				if (CustomizerEntry.CustomMaterial)
				{
					UMaterialInterface* LoadedMaterial = CustomizerEntry.CustomMaterial.Get();
					LoadedMaterial = CustomizerEntry.CustomMaterial.LoadSynchronous();
					UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(LoadedMaterial, this);
					PartsMIDs.Add(FName(TEXT("Hair")), NewMaterial);

					UMaterialInstanceDynamic** MIDPart = PartsMIDs.Find(CustomizerEntry.SkeletalMeshName);
					
					if (CustomizerEntry.CustomMaterial.GetAssetName() == FString(TEXT("MPMI_HAIR_Wavy_23")))
					{				
						(*MIDPart)->SetVectorParameterValue(FName(TEXT("CustomHairColor")), CustomizerEntry.CustomMaterialColor);
						(*FoundPart)->SetMaterial(0, NewMaterial);
					}
					else
					{												
						(*FoundPart)->SetMaterial(0, NewMaterial);															
					}					
				}												
			}
		}
		if (USkeletalMeshComponent** FoundPart = MeshParts.Find(TEXT("Bangs")))
		{			
			if (FoundPart && *FoundPart && CustomizerEntry.SubCustomMesh)
			{				
				USkeletalMesh* LoadedMesh = CustomizerEntry.SubCustomMesh;		
				(*FoundPart)->SetSkeletalMesh(LoadedMesh);
				(*FoundPart)->SetVisibility(true);

				if (CustomizerEntry.CustomMaterial)
				{
					UMaterialInterface* LoadedMaterial = CustomizerEntry.CustomMaterial.Get();
					LoadedMaterial = CustomizerEntry.CustomMaterial.LoadSynchronous();
					UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(LoadedMaterial, this);
					PartsMIDs.Add(FName(TEXT("Bangs")), NewMaterial);

					UMaterialInstanceDynamic** MIDPart = PartsMIDs.Find(TEXT("Bangs"));

					if (CustomizerEntry.CustomMaterial.GetAssetName() == FString(TEXT("MPMI_HAIR_Wavy_23")))
					{						
						(*MIDPart)->SetVectorParameterValue(FName(TEXT("CustomHairColor")), CustomizerEntry.CustomMaterialColor);
						(*FoundPart)->SetMaterial(0, NewMaterial);
					}
					else
					{						
						(*FoundPart)->SetMaterial(0, NewMaterial);
					}	
				}							
			}
			else
			{
				(*FoundPart)->SetVisibility(false);
			}
		}
	}
}

USkeletalMeshComponent* AGASPlayerCharacter::GetMeshPart(FName CategoryName) const
{
	if (USkeletalMeshComponent* const* Found = MeshParts.Find(CategoryName))
	{
		return *Found;
	}
	return nullptr;
}

void AGASPlayerCharacter::Server_SetCustomizationMesh_Implementation(const FGFCustomizerEntry& CustomizerEntry)
{
	if (IsValid(this))
	{
		Multicast_SetCustomizationMesh(CustomizerEntry);
		UE_LOG(LogTemp, Log, TEXT("Server_SetCustomizationMesh executed"));
	}	
}

void AGASPlayerCharacter::Multicast_SetCustomizationMesh_Implementation(const FGFCustomizerEntry& CustomizerEntry)
{
	SetCustomizeMesh(CustomizerEntry);
}

void AGASPlayerCharacter::BindCallbacksToDependencies()
{
	if (bIsBindCallbacks) return;

	if (IsValid(this))
	{
		if (AGASWorldPlayerState* MyPlayerState = GetPlayerState<AGASWorldPlayerState>())
		{
			if (UCustomizerComponent* CustomizerComponent = ICustomizerInterface::Execute_GetCustomizerComponent(MyPlayerState))
			{
				CustomizerComponent->CustomizerList.CustomizerChangedDelegate.AddLambda(
					[this](const FGFCustomizerEntry& CustomizerEntry)
					{
						Server_SetCustomizationMesh(CustomizerEntry);
					});
			
				bIsBindCallbacks = true;
			
				return;
			}
		}
	}
	

	GetWorldTimerManager().SetTimerForNextTick(this, &AGASPlayerCharacter::BindCallbacksToDependencies);
}

void AGASPlayerCharacter::SetMouseSensitivity(float NewSensitivity)
{
	MouseSensitivity = NewSensitivity;
}

float AGASPlayerCharacter::GetMouseSensitivity() const
{
	return MouseSensitivity;
}

void AGASPlayerCharacter::ServerSetNickname_Implementation(const FString& InNickname)
{
	CharacterNickname = InNickname;
}

void AGASPlayerCharacter::OnRep_CharacterNickname()
{
	// TextRenderComponent ����
	//SetNicknameText();

	// NameWidgetComponent ����
	SetNicknameInWidget(CharacterNickname);
}

void AGASPlayerCharacter::SetNicknameInWidget(const FString& InName)
{
	if (!PlayerNameWidgetComponent)
	{
		return;
	}

	// 1) WidgetComponent ������ UserWidget �ν��Ͻ��� ������
	if (UUserWidget* UserWidget = PlayerNameWidgetComponent->GetUserWidgetObject())
	{
		// 2) �츮�� ���� UPlayerNameWidget���� ĳ��Ʈ
		if (UPlayerNameWidget* PlayerNameWidget = Cast<UPlayerNameWidget>(UserWidget))
		{
			// 3) ���� TextBlock�� �̸� ����
			PlayerNameWidget->SetPlayerName(FText::FromString(InName));
			UE_LOG(LogTemp, Log, TEXT("AGASPlayerCharacter: Character Name - %s"), *InName);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("AGASPlayerCharacter: Not UPlayerNameWidget"));
		}
	}
	else
	{
		// WidgetClass�� ���� ���� Ÿ�̹� ������ nullptr�� ��ȯ�� ���� ����
		UE_LOG(LogTemp, Log, TEXT("AGASPlayerCharacter: No UserWidget"));
	}
}

void AGASPlayerCharacter::SetNicknameText()
{
	if (NicknameText)
	{
		NicknameText->SetText(FText::FromString(CharacterNickname));
	}

	UKismetSystemLibrary::PrintString(
		this,                          // WorldContextObject (���� this)
		FString::Printf(TEXT("Nickname: Character - %s"), *CharacterNickname),        // ����� ���ڿ�
		true,                          // bTestEnabled (�����Ϳ����� ���̰� ����)
		true,                          // bPrintToScreen (ȭ�鿡 �������)
		FLinearColor::White,           // �ؽ�Ʈ ����
		5.0f                           // ���� �ð�(��)
	);
}

void AGASPlayerCharacter::MulticastSetNickname_Implementation(const FString& InNickname)
{
	SetNicknameText();
	SetNicknameInWidget(InNickname);
}