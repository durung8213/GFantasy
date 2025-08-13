// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/GASPlayerCharacter.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h" // 디버그용 구체 그리기
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
	//에디터 할당

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

	/* 닉네임 Text 컴포넌트 */
	// 텍스트 컴포넌트 생성 및 루트에 부착
	NicknameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NicknameText"));
	NicknameText->SetupAttachment(RootComponent);
	// 텍스트 위치를 캐릭터 머리 위로 설정
	NicknameText->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	NicknameText->SetHorizontalAlignment(EHTA_Center);
	NicknameText->SetTextRenderColor(FColor::White);
	NicknameText->SetXScale(1.0f);
	NicknameText->SetYScale(1.0f);
	NicknameText->SetWorldSize(20.f);  // 글자 크기
	NicknameText->SetText(FText::FromString(TEXT("Name")));

	// 2) WidgetComponent 생성 (PlayerNameWidgetComponent)
	PlayerNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameWidget"));
	// 캐릭터 머리 위에 띄우기 위한 상대 위치
	PlayerNameWidgetComponent->SetupAttachment(RootComponent);
	PlayerNameWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	// 화면 공간으로 띄우고 싶다면 EWidgetSpace::Screen
	PlayerNameWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// 위젯 크기를 위젯 내부 콘텐츠에 맞추기
	//PlayerNameWidgetComponent->SetDrawAtDesiredSize(true);

	// 라이딩 컴포넌트 생성
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

	// 테스트용 닉네임 설정 (나중에 서버에서 받아오는 걸로 대체)
	//SetNicknameText();
	SetNicknameInWidget(CharacterNickname);
}

// 캐릭터의 락온 시작
void AGASPlayerCharacter::Server_PerformAutoTargeting_Implementation()
{

	if (TargetingState == ETargetingState::LockOn)
		return;		// 락온 중에 새로운 포커싱 금지

	if (bIsMount)
	{
		return;
	}
	// Overlap 범위
	float Radius = 1000.0f;
	// 시야각
	float FOV = 90.0f;
	FVector MyLocation = GetActorLocation();


	TArray<AActor*> HitActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 플레이어 자신은 감지되지 않도록 무시
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	// World가 유효한지 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("PossessedBy: GetWorld() returned null!"));
		return;
	}

	// SphereOverlapActors 실행
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		Radius,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		HitActors
	);


	// 감지된 적을 담은 Target 배열과 자동 포커싱될 BestTarget 설정
	AActor* BestTarget = nullptr;
	float BestDistSq = FLT_MAX;
	Targets.Empty();

	if (bHit)
	{
		for (AActor* Result : HitActors)
		{
			// 오버랩된 액터가 적인지 확인
			if (!Result || !Result->ActorHasTag("Enemy"))
				continue;

			// 오버랩된 적이 플레이어 시야에 있는지 확인
			if (!IsInCameraFOV(Result, FOV))
			{
				UKismetSystemLibrary::PrintString(this, TEXT("FOV FALSE"));
				continue;
			}

			float DistSq = FVector::DistSquared(MyLocation, Result->GetActorLocation());

			// 가장 가까이에 있는 적을 찾기.
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
		//오버랩된 적이 없다면 타겟팅 상태를 None 으로 초기화.
		TargetingState = ETargetingState::None;
	}

	// 갱신된 타겟이 현재 내 타겟이 아니라면 갱신
 	if (BestTarget != CurrentTarget)
	{
		// 현재 타겟 재설정
		CurrentTarget = BestTarget;
		if (CurrentTarget)
		{
			TargetingState = ETargetingState::Focusing;
			UE_LOG(LogTemp, Warning, TEXT("TARGET DETECT : %s"), *CurrentTarget->GetName());
		}

		// 서버용 전투 상태 갱신 호출 추가
		EvaluateCombatState();   // bShouldBeInCombat 계산
		UpdateCombatState();     // 타이머 관리 및 SetCombatState 호출
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

	//해당 배열이 0이 아닐 때, 실행하도록.
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

	// Interact 실행
	if (BestTarget && BestTarget->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		// 해당 액터에 Interaction 기능 수행
		IInteractionInterface::Execute_Interact(BestTarget, this);
	}

}

void AGASPlayerCharacter::Server_SetInputDirection_Implementation(FVector Direction)
{
	CachedInputDirection = Direction;
}


// Upper 애니메이션 멀티로 뿌려주기.
void AGASPlayerCharacter::Multicast_PlayUpperBodyMontage_Implementation(
	UAnimMontage* Montage, bool bBlendOn)
{
	bHasUpperBodyAction = bBlendOn;
	OnRep_UpperBodyBlend();  // 클라에서도 바로 적용

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		if (bBlendOn && Montage)
		{
			// 몽타주 재생 (중복 방지)
			if (!Anim->Montage_IsPlaying(Montage))
				Anim->Montage_Play(Montage, 1.0f);
		}
		else if (!bBlendOn && Montage)
		{
			// 애니메이션 정지
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

// 현재 타겟 상태	에 따라 정보 업데이트
void AGASPlayerCharacter::OnRep_CurrentTarget()
{
	if (IsValid(CurrentTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("TARGET DETECT : %s"), *CurrentTarget->GetName());
	}
	
	// 전투 / 비전투 상태 체크 시작
	EvaluateCombatState();	// 전투 상태 진입 여부 체크
	UpdateCombatState();	// 타겟 없으면 타이머 시작.
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
		// 현재 IMC 저장하기1
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

	// 카메라에서 타겟으로 향하는 벡터를 구함
	FVector ToTarget = (TargetActor->GetActorLocation() - FollowCamera->GetComponentLocation()).GetSafeNormal();

	// 카메라의 앞 방향 벡터를 구함
	FVector CameraForward = FollowCamera->GetForwardVector();

	// 두 벡터의 내적을 구함                                                                                                                                                   
	float DotProduct = FVector::DotProduct(CameraForward, ToTarget);

	// 둘 사이의 각도를 구함.
	float Radians = FMath::Acos(DotProduct);

	float Degrees = FMath::RadiansToDegrees(Radians);

	//현재 각도가 시야각 안이면 true를 반환
	return Degrees <= (FOVDegrees * 0.5f);	// 절반 시야각 (좌우) 반환
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
	//현재 락온 상태라면
	TargetingState = ETargetingState::Focusing;

	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	CameraBoom->SocketOffset = FVector(0.f, 150.f, 30.f);

	// 락온 해제
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

	
	// 어빌리티 실행
	PlayerASC->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Dodge));

}

void AGASPlayerCharacter::HandleHitReact(AActor* HitInstigator, float DamageAmount, bool bStrong)
{
	// 적에게 유효한 타격이 있을 때, 이 함수가 실행됨. 
	UE_LOG(LogTemp, Warning, TEXT("HandleHit"));
	// 1. 방향 계산
	FVector Dir = (HitInstigator->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	FVector Forward = GetActorForwardVector().GetSafeNormal2D();

	float Dot = FVector::DotProduct(Forward, Dir);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
	FVector Cross = FVector::CrossProduct(Forward, Dir);

	// 2. 태그 결정

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


	// 3. 어빌리티 실행
	UE_LOG(LogTemp, Warning, TEXT("GE Ability Start"));

	if (PlayerASC)
	{
		PlayerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ReactTag));
	}

}


void AGASPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타겟이 존재하고, LockOn 상태라면
	if (TargetingState == ETargetingState::LockOn && CurrentTarget)
	{
		PC = Cast<AGASWorldPlayerController>(GetController());
		if (!PC)
		{
			UE_LOG(LogTemp, Warning, TEXT("No PC"));
			return;
		}

		// 현재 타겟이 죽었을 때, 해당 Tag가 없으면 해제하도록
		if (CurrentTarget->IsActorBeingDestroyed() || !IsValid(CurrentTarget) ||
			!CurrentTarget->ActorHasTag("Enemy"))
		{
			// 락온 해제
			ReleaseLockOn();
			return;
		}

		float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

		if (Distance > 1500.f)
		{
			// 적과 내 사이의 거리가 멀면 Target 해제
			ReleaseLockOn();
			return;
		}

		// 내 위치 기준, 타겟 위치 방향 계산
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = CurrentTarget->GetActorLocation();

		FRotator DesiredRot = (TargetLoc - MyLoc).Rotation();
		DesiredRot.Pitch = -7.f;
		DesiredRot.Roll = 0.f;

		FRotator CurrentRot = PC->GetControlRotation();
		//보간 회전
		FRotator InterpRot = FMath::RInterpTo(CurrentRot, DesiredRot, DeltaTime, CameraRotationInterpSpeed);

		// 카메라 회전
		PC->SetControlRotation(InterpRot);
	}
	else
	{
	}

	// 닉네임이 항상 카메라를 바라보도록
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
		// IMC 적용하고 캐싱하기.
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

	
	// 서버에서만 ASC 초기화 및 능력치 부여
	if (HasAuthority())
	{
		bIsMount = false;

		// 타겟팅을 찾기 위한 타이머 실행
		UE_LOG(LogTemp, Warning, TEXT("Interaction Detect Start"));
		GetWorldTimerManager().SetTimer(
			TargetingTH,
			this,
			&AGASPlayerCharacter::Server_PerformAutoTargeting,
			0.25f,
			true
		);
		
		// 상호작용을 위한 타이머 실행
		UE_LOG(LogTemp, Warning, TEXT("DETECT START"));
		GetWorldTimerManager().SetTimer(
			InteractionTH,
			this,
			&AGASPlayerCharacter::DetectInteractableUI,
			0.3f,	// 매 3초마다 검사
			true
		);



		AGASBasePlayerState* PS = GetPlayerState<AGASBasePlayerState>();
		if (PS && PS->GetAbilitySystemComponent())
		{
			PlayerASC = Cast<UPlayerCharacterGASC>(PS->GetAbilitySystemComponent());
			PlayerASC->InitAbilityActorInfo(PS, this);

			// 단 한번만 실행되도록 설정.
			if (!PS->bIsInitialize)
			{
				PS->bIsInitialize = true;

				//ASC 초기화 완료 능력/ 효과 적용
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
			// DataSet에 설정된 어빌리티 모두 부여.
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
					bIsRespawned = false;	// 자동 Idle 전환을 위한 상태 초기화
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

	// 커스터마이즈 컴포넌트 델리게이트 설정
	BindCallbacksToDependencies();

	if (AGASWorldPlayerState* PS = GetPlayerState<AGASWorldPlayerState>())
	{
		UCustomizerComponent* CustomizerComponent = ICustomizerInterface::Execute_GetCustomizerComponent(PS);
		for (const FGFCustomizerEntry& Entry : CustomizerComponent->GetCustomizerEntries())
		{
			SetCustomizeMesh(Entry);
		}
	}	
	
	// PlayerState의 값이 변할 때 마다, InitAbilityActorInfo를 실행해준다.

	UE_LOG(LogTemp, Log, TEXT("OnRep_PlayerState called"));

	// PlayerState의 값이 변할 때 마다, InitAbilityActorInfo를 실행해준다.
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
		// 클라이언트 용 초기화.
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

	// 기본 캐릭터 이동
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
		// 락온 중에는 카메라를 움직이지 못하도록 설정.
		return;
	}


	if (Controller != nullptr)
	{
		AddControllerYawInput(v.X * MouseSensitivity);
		AddControllerPitchInput(v.Y * MouseSensitivity);
	}
}

// 마우스 좌클릭
// 공격키를 누르게 되면
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

	// 다른 어빌리티가 실행중일 때,
	if (PlayerASC->HasMatchingGameplayTag(BlockTag))
	{
		// 해당 어빌리티가 콤보 태그도 없다면 return
		if (!PlayerASC->HasMatchingGameplayTag(ComboTag))
		{
			UKismetSystemLibrary::PrintString(this, TEXT("Combo Blocked"));
			return;
		}
	}

	// 어빌리티가 실행되어 있지 않다면 어빌리티를 실행하고 InputPressed를 실행
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

// R 버튼 누를 시, 락온 모드 스위칭
void AGASPlayerCharacter::HandleLockOnToggle()
{
	// 이미 락온 모드라면 해제
	if (TargetingState == ETargetingState::LockOn)
	{
		ReleaseLockOn();
		return;
	}

	// 현재 포커싱상태이고, 타겟이 유효할 때 락온 모드 진입
	if (TargetingState == ETargetingState::Focusing && IsValid(CurrentTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("In Lock On Mode"));
		// 락온 모드 돌입

		// 카메라 위치 상승
		CameraBoom->TargetArmLength = 500.f;
		CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		CameraBoom->SocketOffset = FVector(0.f, 0.f, 200.f);


		// 상태 변경
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
	// F 키 누를 시 활성화.
	Server_TryInteract();
}

void AGASPlayerCharacter::OnQPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("On Q Pressed"));
	// Q 버튼 누를 시, 스킬 실행

	if (CurrentTarget != nullptr)
	{
		SkillBarComp->ActivateSlot(EAbilitySlot::SlotQ);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Q Skill UnUse : No Current Target"));
}

void AGASPlayerCharacter::OnEPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("On E Pressed"));
	// E 버튼 누를 시, 스킬 실행
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

// 대쉬 시작
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

	// World가 유효한지 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("PossessedBy: GetWorld() returned null!"));
		return;
	}

	// 상호작용을 위한 오버랩 실행
	UKismetSystemLibrary::SphereOverlapActors(
		World,
		GetActorLocation(),
		500.f,
		{ UEngineTypes::ConvertToObjectType(ECC_Pawn) },
		nullptr,
		Ignore,
		HitActors
	);

	// 구조 수정 요망
	OverlappedInteractables.Empty();

	for (AActor* Actor : HitActors)
	{
		if (!Actor->ActorHasTag(FName("Interactable")))
			continue;

		OverlappedInteractables.AddUnique(Actor);

		// 각 대상에 대한 UI 표시
		// 수정 요망
		// 상호작용가능한 것들에게는 모두 구현될 함수가 필요.
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

// IMC 적용하기
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

// EnhancedInput SubSystem 가져오기.
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

// 현재 전투 상태인지를 체크.
void AGASPlayerCharacter::UpdateCombatState()
{
	// 전투 상태라면 체크할 필요 X
	if (CombatState != ECombatState::InCombat)
		return;

	if (CurrentTarget == nullptr)
	{	
		//현재 타겟이 없고, 현재 전투 상태를 체크하는 타이머가 실행되지 않고 있다면 실행.
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
		// 타겟이 다시 생기면 타이머 종료
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
	// 여기서 어빌리티 실행
			
	// 애니메이션 어빌리티 실행
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

// 타이머 시간 조건 활성화 : 비전투 모드 전환 함수
void AGASPlayerCharacter::HandleCombatStateTimeOut()
{
	if (CombatState == ECombatState::InCombat && CurrentTarget == nullptr)
	{
		SetCombatState(ECombatState::NonCombat);
		//ApplyIMC(IMC_UnArmed,0);
		UE_LOG(LogTemp,Warning,TEXT("Current CombatState : NON COMBAT")); 
	}
}

// 전투 상태 체크 함수
void AGASPlayerCharacter::EvaluateCombatState()
{
	// 무기 여부와 Target 여부로 전투 모드 판단
	const bool bHasWeapon = (CurrentWeapon != nullptr);
	const bool bHasTarget = (CurrentTarget != nullptr);
	const bool bShouldBeInCombat = bHasWeapon && bHasTarget;

	// 현재 Combat 상태인지 아닌지를 판단
	const ECombatState NewState = bShouldBeInCombat?ECombatState::InCombat : ECombatState::NonCombat;

	if (bShouldBeInCombat)
	{
		// 전투 모드인데, 전투모드가 되어있지 않다면
		if (CombatState != ECombatState::InCombat)
		{
			// 전투 모드로 전환.
			SetCombatState(ECombatState::InCombat);
			UE_LOG(LogTemp,Warning,TEXT("Combat State IMC Apply"));
		}

		// 전투 모드 일 때, 타겟이 있을 때는 항상 비전투 감지 타이머 초기화
		GetWorld()->GetTimerManager().ClearTimer(CombatStateTimer);
	}
	
}

void AGASPlayerCharacter::AttachWeaponToSocket(FName SocketName) const
{
	if (CurrentWeapon)
	{
		// 1. 무기 뗴어내기
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. 미리 소켓 위치로 무기 이동
		FTransform SocketTransform = GetMesh()->GetSocketTransform(SocketName);
		CurrentWeapon->SetActorTransform(SocketTransform);

		// 3. 소켓에 붙이기
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

	// 무기가 있다면 삭제
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
	// TextRenderComponent 설정
	//SetNicknameText();

	// NameWidgetComponent 설정
	SetNicknameInWidget(CharacterNickname);
}

void AGASPlayerCharacter::SetNicknameInWidget(const FString& InName)
{
	if (!PlayerNameWidgetComponent)
	{
		return;
	}

	// 1) WidgetComponent 내부의 UserWidget 인스턴스를 가져옴
	if (UUserWidget* UserWidget = PlayerNameWidgetComponent->GetUserWidgetObject())
	{
		// 2) 우리가 만든 UPlayerNameWidget으로 캐스트
		if (UPlayerNameWidget* PlayerNameWidget = Cast<UPlayerNameWidget>(UserWidget))
		{
			// 3) 내부 TextBlock에 이름 설정
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
		// WidgetClass나 위젯 생성 타이밍 문제로 nullptr이 반환될 수도 있음
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
		this,                          // WorldContextObject (보통 this)
		FString::Printf(TEXT("Nickname: Character - %s"), *CharacterNickname),        // 출력할 문자열
		true,                          // bTestEnabled (에디터에서만 보이게 할지)
		true,                          // bPrintToScreen (화면에 출력할지)
		FLinearColor::White,           // 텍스트 색상
		5.0f                           // 지속 시간(초)
	);
}

void AGASPlayerCharacter::MulticastSetNickname_Implementation(const FString& InNickname)
{
	SetNicknameText();
	SetNicknameInWidget(InNickname);
}