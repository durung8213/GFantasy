// Fill out your copyright notice in the Description page of Project Settings.
#include "GameMode/WorldGameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/AttributeSet/GASCharacterAS.h"
#include "PlayerState/GASBasePlayerState.h"
#include "Controller/GASWorldPlayerController.h"
#include "Character/GASPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SaveGame/GFSaveGame.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "PlayerState/GASWorldPlayerState.h"

AWorldGameMode::AWorldGameMode()
{
}

void AWorldGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AWorldGameMode::SetAllPlayerNicknameText()
{
    UWorld* World = GetWorld();

    if (World == nullptr)
    {
        return;
    }

    // ��� PlayerController ��ȸ
    for (FConstPlayerControllerIterator ArrPC = World->GetPlayerControllerIterator(); ArrPC; ++ArrPC)
    {
        APlayerController* PC = ArrPC->Get();
        if (PC == nullptr)
        {
            continue;
        }

        // ���� ����� ���� ��Ʈ�ѷ���� ĳ��Ʈ
        AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
        if (MyPC)
        {
            // PC�� ������ Pawn(ĳ����)�� �����ͼ� �г��� ����
            APawn* Pawn = MyPC->GetPawn();
            AGASPlayerCharacter* MyChar = Cast<AGASPlayerCharacter>(Pawn);
            if (MyChar != nullptr)
            {
                // ��: ĳ���Ϳ� ����� ReplicatedNickname�� �ٽ� ����
                MyChar->SetNicknameText();
                MyChar->SetNicknameInWidget(MyChar->GetCharacterNickname());
            }
        }

    }
}

void AWorldGameMode::RespawnPlayerAtVillage(AController* Controller)
{
	if (!Controller)
		return;

    AGASPlayerCharacter* Pawn = Cast<AGASPlayerCharacter>(Controller->GetPawn());
    if(!Pawn)   return;

    // �� ��ġ �����ϱ�.
    Pawn->SetActorLocation(FindVillagePlayerStart()->GetActorLocation());
    Pawn->SetActorRotation(FindVillagePlayerStart()->GetActorRotation());

    Pawn->bIsDead = false;
    Pawn->bIsRespawned = true;

    //  AS ���� ����
	if (AGASBasePlayerState* PS = Controller->GetPlayerState<AGASBasePlayerState>())
	{
		if (UGASCharacterAS* CharAS = PS->GetCharacterAttributeSet())
		{
			// ü���� �ִ� ü������ ������ش�.
			CharAS->SetHealth(CharAS->GetMaxHealth());
		}
        if (UAbilitySystemComponent* ASC = PS->GetPlayerCharacterGASC())
        {
            ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Dead.Player"));
        }
	}

    // ĳ���� �Է� ���
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {

        // ī�޶� ����
        PC->SetViewTargetWithBlend(Pawn, 0.f);

        // Possess ��Ȯ��
        if (Controller->GetPawn() != Pawn)
        {
            Controller->Possess(Pawn);
            UE_LOG(LogTemp, Warning, TEXT("Possess From Dead"));
        }
        
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
        Pawn->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

        UE_LOG(LogTemp, Warning, TEXT("Respawn Success: %s"), *Pawn->GetName());
    }
}
