// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Character/GASBaseCharacter.h"
#include "DataAsset/GASAbilitySet.h"
#include "GFantasyEnums.h"
#include "Interface/InventoryInterface.h"
#include "EnhancedInputSubsystems.h"
#include "Customization/CustomizerComponent.h"
#include "Interface/CustomizerInterface.h"
#include "GASPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AGASWorldPlayerController;
class ARidingActorBase;
class UPlayerCharacterGASC;
class URidingComponent;
class AWeaponBase;
class UEnhancedInputSubsystems;

/**
 * 
 */
UCLASS()
class GFANTASY_API AGASPlayerCharacter : public AGASBaseCharacter, public IInventoryInterface, public ICustomizerInterface
{
	GENERATED_BODY()
	
public:
	AGASPlayerCharacter();
	
	// ������Ʈ �ʱ�ȭ ���� �Լ�
	virtual void PostInitializeComponents() override;

	/* ���� RPC */

	// �������� Ÿ���� ã�� ����
	UFUNCTION(Server, Reliable)
	void Server_PerformAutoTargeting();

	// �������� Lock On ����
	UFUNCTION(Server, Reliable)
	void Server_TryLockOn();

	// �������� F ��ȣ�ۿ� ����
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();

	UFUNCTION(Server, Reliable)
	void Server_SetInputDirection(FVector Direction);

	UFUNCTION(Server, Reliable)
	void Server_TryActivateLimitBreak();

	UFUNCTION(Server, Reliable)
	void Server_StartSprint();

	UFUNCTION(Server, Reliable)
	void Server_StopSprint();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayUpperBodyMontage(UAnimMontage* Montage, bool bBlendOn);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AttachWeaponToSocket(FName SocketName)const;
	
	/* On Rep */

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION()
	void OnRep_CurrentTarget();

	UFUNCTION()
	void OnRep_TargetingState();

	UFUNCTION()
	void OnRep_UpperBodyBlend();

	UFUNCTION()
	void OnRep_IsDead();

	UFUNCTION()
	void OnRep_bIsRespawned();
	
	UFUNCTION()
	void OnRep_IsUIShow();
	// ���� ž�� ���� 
	UFUNCTION()
	void OnRep_IsMount();

	// ���� ���°� �ٲ𶧸��� Ŭ���̾�Ʈ���� ����� �Լ�
	UFUNCTION()
	void OnRep_CombatState();

	// ���� ��� ���°� �ٲ𶧸��� ����� �Լ�
	UFUNCTION()
	void OnRep_CurrentWeapon();
	
	// ���� ī�޶� FOV �ȿ� �ִ��� ������ ���� üũ
	UFUNCTION()
	bool IsInCameraFOV(AActor* TargetActor, float FOVDegrees) const;

	// ���� ����
	UFUNCTION()
	void TryLockOn();

	// ���� ����
	UFUNCTION()
	void ReleaseLockOn();

	// ȸ�� �׼�
	UFUNCTION()
	void DodgeAction();

	// �ǰ� �׼�
	UFUNCTION()
	void HandleHitReact(AActor* HitInstigator, float DamageAmount, bool bStrong);

	// Ż �� ����� �����ϴ� ���̵� ������Ʈ
	URidingComponent* GetRidingComponent() const { return RidingComponent; }

	// ������ ȣ���ؼ� �г��� ����, ����
	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& InNickname);

	// ������ ȣ���ϸ� ��� Ŭ���̾�Ʈ���� ����Ǵ� multicast RPC
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetNickname(const FString& InNickname);

	UFUNCTION()
	void OnRep_CharacterNickname();

	void SetNicknameText();
	void SetNicknameInWidget(const FString& InName);


	UFUNCTION(BlueprintCallable)
	FString GetCharacterNickname();

	// IMC �����ϱ� �� ĳ���ϱ�
	UFUNCTION(BlueprintCallable)
	void ApplyIMC(UInputMappingContext* NewIMC, int32 Priority);

	// ���� ���⸦ �ش� ���Ͽ� �����ϴ� �Լ�
	UFUNCTION()
	void AttachWeaponToSocket(FName SocketName) const;

	// ���� ���� ���� ��������
	ECombatState GetCombatState() const { return CombatState; }

	// ���� ���� ��������.
	UFUNCTION()
	AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	// Ÿ�̸ӷ� ����� �Լ�. ����ؼ� ���� / ������ ���¸� Ȯ���Ѵ�.
	UFUNCTION()
	void UpdateCombatState();

	// ���� ���� ���� ����
	UFUNCTION()
	void SetCombatState(ECombatState NewState);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	/* �Է� ó�� �ڵ鷯 */

	// �Է� �ݹ�
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// ���� ��ư Ŭ���� ����� �Լ�
	void OnAttackPressed();

	// ���� ��ư Released ��, ����� �Լ�
	void OnAttackReleased();

	// ���Ÿ� ���� ��ư Ŭ�� �� ����
	void OnRangedAttackPressed();

	// ���� ��� ��� ����
	void HandleLockOnToggle();

	// F ��ư Ŭ���� ����� �Լ� ( ���ͷ��� )
	void OnInteractPressed();

	// Q ��ư ��ų ����
	void OnQPressed();

	// E ��ư ��ų ����
	void OnEPressed();

	// X ��ư : Ż �� ȣ��
	void OnXPressed();

	// T ��ư : ����Ʈ �극��ũ ȣ��
	void OnTPressed();

	// Shift : �뽬 ����
	void OnSprintStarted();

	// Shift : �뽬 ����
	void OnSprintFinished();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ���� ���°� ���ӵ��� ���� ��, ������ ���·� ��ȯ�ϴ� �Լ�
	void HandleCombatStateTimeOut();

	// ���� ���¿� �°� IMC ���� �Լ�
	void EvaluateCombatState();
private:

	// ��ȣ�ۿ� �� ������	( UI �� ���� )
	void DetectInteractableUI(); 

	// ����Ʈ �극��ũ ����
	void TryActivateLimitBreak();

	// ���� ĳ������ SubSystem ��������.
	UEnhancedInputLocalPlayerSubsystem* GetEISubSystem();



public:

	// IMC ���
	
	// 0. ���Ⱑ ���� �⺻ ���
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_UnArmed;

	// 1. �ҵ� ���� ���
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_SwordArmed;

	// 2. Ż �� ���
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_Riding;

	// 3. ��ȣ�ۿ� ���
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_Interaction;

	
	/* IA ��� */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Attack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_LockOn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_RangedAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Dodge;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Interaction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Q;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_E;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_Dash;

	
	// ����Ʈ �극��ũ ��ų
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_T;

	// Ż�� ȣ��
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_X;






	/* ī�޶� ���� */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraInterpSpeed = 8.0f;

	//ASC ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UPlayerCharacterGASC> PlayerASC;

	// AttributeSet ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TObjectPtr<UGASAbilitySet> PlayerAbilitySet;

	//���� Ž���� ����
	UPROPERTY()
	TArray<AActor*> Targets;

	// ���� ĳ���Ͱ� ������ �ִ� Ÿ�� ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentTarget, Category = "Targeting")
	AActor* CurrentTarget = nullptr;


	// ���� ĳ���Ͱ� LockOn ������ üũ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", Replicated)
	bool bIsLockOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState;
	
	// ī�޶� ȸ�� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float CameraRotationInterpSpeed = 7.5f;

	// ���� ī�޶� ���� ( ��Ŀ�� / ���� )
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting", ReplicatedUsing = OnRep_TargetingState)
	ETargetingState TargetingState = ETargetingState::None;

	// ��ź Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> RangedProjectileClass;

	/* ������ */

	// ĳ���Ͱ� �������� ���� ���� �Է��� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FVector CachedInputDirection;


	/* ��ȣ�ۿ� */
	// ��ȣ�ۿ� Ž��
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	class USphereComponent* InteractionDetector;


	/* Ż �� */ 

	// ������ Ż �� ������ Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Riding")
	TSubclassOf<ARidingActorBase> RidingClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Riding")
	URidingComponent* RidingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsMount)
	bool bIsMount = false;


	// �������� ���͵��� �迭
	UPROPERTY()

	TArray<AActor*> OverlappedInteractables;

	/* ��ų ������Ʈ */
	
	// ��ų ���� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkillBarComponent* SkillBarComp;

	// ���� ��ü �׼� �÷���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_UpperBodyBlend)
	bool bHasUpperBodyAction = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bIsRespawned)
	bool bIsRespawned = false;
	
	// ���� �������� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_CurrentWeapon)
	AWeaponBase* CurrentWeapon;

	// IMC �� ���� Bool ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_IsUIShow)
	bool bIsUIShow;

	virtual void OnNetCleanup(class UNetConnection* Connection) override;

	// ĳ���� ���� ����� ��, ����Ǵ� �Լ�
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	AGASWorldPlayerController* PC;

	// �̸� ���� ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name|UI")
	class UWidgetComponent* PlayerNameWidgetComponent;

	// �⺻���� ����� ���� Ŭ����
	UPROPERTY(EditDefaultsOnly, Category = "Name|UI")
	TSubclassOf<class UPlayerNameWidget> DefaultNameWidgetClass;

	// �г���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UTextRenderComponent* NicknameText;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CharacterNickname)
	FString CharacterNickname;

private:
	// Ÿ���� �� Ÿ�̸� �ڵ�
	FTimerHandle TargetingTH;

	// ��ȣ�ۿ� �� Ÿ�̸� �ڵ�
	FTimerHandle InteractionTH;

	UInputMappingContext* CachedIMC = nullptr;

	// �� �ѹ��� �ʱ� ������ �ʱ�ȭ �ϵ��� ����.
	bool bIsInitialize = false;

/* Ŀ���͸���� ���� �ڵ� */
public:

	UFUNCTION(BlueprintCallable)
	void SetCustomizeMesh(const FGFCustomizerEntry& CustomizerEntry);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Customize")
	USkeletalMeshComponent* GetMeshPart(FName CategoryName) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FName,	USkeletalMeshComponent*> MeshParts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FName, UMaterialInstanceDynamic*> PartsMIDs;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> MainAnimInstanceClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> HairAnimInstanceClass;	

	bool bIsBindCallbacks = false;
	
private:

	UFUNCTION(Server, Reliable)
	void Server_SetCustomizationMesh(const FGFCustomizerEntry& CustomizerEntry);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCustomizationMesh(const FGFCustomizerEntry& CustomizerEntry);

	/*��������Ʈ ���ε带 �ϴ� �Լ�*/
	void BindCallbacksToDependencies();




/////////////////////////// ���콺 ���� ���� ///////////////////////////
public:

	// ���� ����
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMouseSensitivity(float NewSensitivity);

	// ���� ��ȸ
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetMouseSensitivity() const;

private:
	float MouseSensitivity = 1.0f;

	float DefaultSpeed = 600.f;
	float SprintSpeed = 850.f;

	// ���� ���� �����ϴ� Ÿ�̸� �ڵ�
	FTimerHandle CombatStateTimer;

	// ������ ���·� ��ȯ�ϱ���� �ɸ��� �ð�
	float CombatStateCooldownTime = 8.0f;

//---------------- ī�޶� ���� ����
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Sequence")
	void CustomizeCameraOff();
};
