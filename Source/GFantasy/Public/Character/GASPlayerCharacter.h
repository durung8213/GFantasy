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
	
	// 컴포넌트 초기화 전용 함수
	virtual void PostInitializeComponents() override;

	/* 서버 RPC */

	// 서버에서 타겟팅 찾기 실행
	UFUNCTION(Server, Reliable)
	void Server_PerformAutoTargeting();

	// 서버에서 Lock On 실행
	UFUNCTION(Server, Reliable)
	void Server_TryLockOn();

	// 서버에서 F 상호작용 시작
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
	// 현재 탑승 상태 
	UFUNCTION()
	void OnRep_IsMount();

	// 전투 상태가 바뀔때마다 클라이언트에서 실행될 함수
	UFUNCTION()
	void OnRep_CombatState();

	// 현재 장비 상태가 바뀔때마다 실행될 함수
	UFUNCTION()
	void OnRep_CurrentWeapon();
	
	// 현재 카메라 FOV 안에 있는지 없는지 여부 체크
	UFUNCTION()
	bool IsInCameraFOV(AActor* TargetActor, float FOVDegrees) const;

	// 락온 수행
	UFUNCTION()
	void TryLockOn();

	// 락온 해제
	UFUNCTION()
	void ReleaseLockOn();

	// 회피 액션
	UFUNCTION()
	void DodgeAction();

	// 피격 액션
	UFUNCTION()
	void HandleHitReact(AActor* HitInstigator, float DamageAmount, bool bStrong);

	// 탈 것 기능을 관리하는 라이딩 컴포넌트
	URidingComponent* GetRidingComponent() const { return RidingComponent; }

	// 서버가 호출해서 닉네임 설정, 복제
	UFUNCTION(Server, Reliable)
	void ServerSetNickname(const FString& InNickname);

	// 서버가 호출하면 모든 클라이언트에서 실행되는 multicast RPC
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetNickname(const FString& InNickname);

	UFUNCTION()
	void OnRep_CharacterNickname();

	void SetNicknameText();
	void SetNicknameInWidget(const FString& InName);


	UFUNCTION(BlueprintCallable)
	FString GetCharacterNickname();

	// IMC 변경하기 및 캐싱하기
	UFUNCTION(BlueprintCallable)
	void ApplyIMC(UInputMappingContext* NewIMC, int32 Priority);

	// 현재 무기를 해당 소켓에 장착하는 함수
	UFUNCTION()
	void AttachWeaponToSocket(FName SocketName) const;

	// 현재 전투 상태 가져오기
	ECombatState GetCombatState() const { return CombatState; }

	// 현재 무기 가져오기.
	UFUNCTION()
	AWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

	// 타이머로 실행될 함수. 계속해서 전투 / 비전투 상태를 확인한다.
	UFUNCTION()
	void UpdateCombatState();

	// 현재 전투 상태 설정
	UFUNCTION()
	void SetCombatState(ECombatState NewState);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	/* 입력 처리 핸들러 */

	// 입력 콜백
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// 공격 버튼 클릭시 실행될 함수
	void OnAttackPressed();

	// 공격 버튼 Released 시, 실행될 함수
	void OnAttackReleased();

	// 원거리 공격 버튼 클릭 시 실행
	void OnRangedAttackPressed();

	// 락온 토글 기능 진행
	void HandleLockOnToggle();

	// F 버튼 클릭시 실행될 함수 ( 인터랙션 )
	void OnInteractPressed();

	// Q 버튼 스킬 실행
	void OnQPressed();

	// E 버튼 스킬 실행
	void OnEPressed();

	// X 버튼 : 탈 것 호출
	void OnXPressed();

	// T 버튼 : 리미트 브레이크 호출
	void OnTPressed();

	// Shift : 대쉬 시작
	void OnSprintStarted();

	// Shift : 대쉬 종료
	void OnSprintFinished();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 전투 상태가 지속되지 않을 시, 비전투 상태로 전환하는 함수
	void HandleCombatStateTimeOut();

	// 전투 상태에 맞게 IMC 적용 함수
	void EvaluateCombatState();
private:

	// 상호작용 용 오버랩	( UI 용 감지 )
	void DetectInteractableUI(); 

	// 리미트 브레이크 실행
	void TryActivateLimitBreak();

	// 현재 캐릭터의 SubSystem 가져오기.
	UEnhancedInputLocalPlayerSubsystem* GetEISubSystem();



public:

	// IMC 등록
	
	// 0. 무기가 없는 기본 모드
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_UnArmed;

	// 1. 소드 장착 모드
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_SwordArmed;

	// 2. 탈 것 모드
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_Riding;

	// 3. 상호작용 모드
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* IMC_Interaction;

	
	/* IA 등록 */
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

	
	// 리미트 브레이크 스킬
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_T;

	// 탈것 호출
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* IA_X;






	/* 카메라 설정 */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraInterpSpeed = 8.0f;

	//ASC 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UPlayerCharacterGASC> PlayerASC;

	// AttributeSet 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TObjectPtr<UGASAbilitySet> PlayerAbilitySet;

	//현재 탐지된 적들
	UPROPERTY()
	TArray<AActor*> Targets;

	// 현재 캐릭터가 가지고 있는 타겟 적
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentTarget, Category = "Targeting")
	AActor* CurrentTarget = nullptr;


	// 현재 캐릭터가 LockOn 중인지 체크
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting", Replicated)
	bool bIsLockOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState;
	
	// 카메라 회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float CameraRotationInterpSpeed = 7.5f;

	// 현재 카메라 상태 ( 포커싱 / 락온 )
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting", ReplicatedUsing = OnRep_TargetingState)
	ETargetingState TargetingState = ETargetingState::None;

	// 포탄 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AActor> RangedProjectileClass;

	/* 구르기 */

	// 캐릭터가 마지막에 누른 원본 입력을 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FVector CachedInputDirection;


	/* 상호작용 */
	// 상호작용 탐지
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	class USphereComponent* InteractionDetector;


	/* 탈 것 */ 

	// 스폰할 탈 것 액터의 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Riding")
	TSubclassOf<ARidingActorBase> RidingClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Riding")
	URidingComponent* RidingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsMount)
	bool bIsMount = false;


	// 오버랩된 액터들의 배열
	UPROPERTY()

	TArray<AActor*> OverlappedInteractables;

	/* 스킬 컴포넌트 */
	
	// 스킬 관리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USkillBarComponent* SkillBarComp;

	// 블렌드 상체 액션 플래그
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_UpperBodyBlend)
	bool bHasUpperBodyAction = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_bIsRespawned)
	bool bIsRespawned = false;
	
	// 현재 장착중인 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_CurrentWeapon)
	AWeaponBase* CurrentWeapon;

	// IMC 를 위한 Bool 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_IsUIShow)
	bool bIsUIShow;

	virtual void OnNetCleanup(class UNetConnection* Connection) override;

	// 캐릭터 접속 종료될 때, 실행되는 함수
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	AGASWorldPlayerController* PC;

	// 이름 위젯 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Name|UI")
	class UWidgetComponent* PlayerNameWidgetComponent;

	// 기본으로 사용할 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Name|UI")
	TSubclassOf<class UPlayerNameWidget> DefaultNameWidgetClass;

	// 닉네임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UTextRenderComponent* NicknameText;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CharacterNickname)
	FString CharacterNickname;

private:
	// 타겟팅 용 타이머 핸들
	FTimerHandle TargetingTH;

	// 상호작용 용 타이머 핸들
	FTimerHandle InteractionTH;

	UInputMappingContext* CachedIMC = nullptr;

	// 딱 한번만 초기 스텟을 초기화 하도록 설정.
	bool bIsInitialize = false;

/* 커스터마이즈를 위한 코드 */
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

	/*델리게이트 바인드를 하는 함수*/
	void BindCallbacksToDependencies();




/////////////////////////// 마우스 감도 설정 ///////////////////////////
public:

	// 감도 설정
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMouseSensitivity(float NewSensitivity);

	// 감도 조회
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetMouseSensitivity() const;

private:
	float MouseSensitivity = 1.0f;

	float DefaultSpeed = 600.f;
	float SprintSpeed = 850.f;

	// 전투 상태 감지하는 타이머 핸들
	FTimerHandle CombatStateTimer;

	// 비전투 상태로 변환하기까지 걸리는 시간
	float CombatStateCooldownTime = 8.0f;

//---------------- 카메라 변경 관련
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Sequence")
	void CustomizeCameraOff();
};
