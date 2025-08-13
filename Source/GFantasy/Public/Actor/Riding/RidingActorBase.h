// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "RidingActorBase.generated.h"

class AGASPlayerCharacter;
class UInputComponent;
class UEnhancedInputComponent; 
class UInputMappingContext;
class UInputAction;
class UEnhancedInputSubsystems;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UMaterialInstanceDynamic;

UCLASS()
class GFANTASY_API ARidingActorBase : public ACharacter
{
	GENERATED_BODY()
	
public:	

	ARidingActorBase();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void SetRider(AGASPlayerCharacter* InRider) { Rider = InRider; }

	UFUNCTION(Server, Reliable)
	void Server_RequestUnMount();

	// ��Ÿ���� ������ ���� ����
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartDissolveIn();

	// ������� ������ ���� ���� 
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StartDissolveOut();

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(Client, Reliable)
	void Client_ApplyRidingIMC();

	UFUNCTION(BlueprintCallable)
	void OnXPressed();

	virtual void OnNetCleanup(class UNetConnection* Connection) override;
	
	// ĳ���� ���� ����� ��, ����Ǵ� �Լ�
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	// �Է� ó��
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION()
	void OnRep_Rider();

	UFUNCTION()
	void UpdateDissolveIn();

	UFUNCTION()
	void UpdateDissolveOut();
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* RidingMesh; 

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Rider)
	AGASPlayerCharacter* Rider;


	/* ī�޶� ���� */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* RidingCamera;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DissolveMats;
	
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> RiderDissolveMats;

	/* �޽� ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Armor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Props;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	// UStaticMeshComponent* Horn;

protected:

	// �Է� �׼�
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* RidingIMC;

	UPROPERTY(EditDefaultsOnly, Category = "VALUE")
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "VALUE")
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "VALUE")
	UInputAction* IA_UnMount;

private:
	
	// ������ ���൵
	float FadeAlpha = 1.0f;

	// Ÿ�̸� �ڵ�
	FTimerHandle FadeTimer;

	/////////////////////////// ���콺 ���� ���� ///////////////////////////
public:

	// ���� ����
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMouseSensitivity(float NewSensitivity);

	// ���� ��ȸ
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetMouseSensitivity() const;

	// ���� ������ ������, Or ź ���·� ���� ������.
	UPROPERTY()
	bool bIsBeingUnmounted = false;

private:
	float MouseSensitivity = 1.0f;

};

