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

    // 모든 PlayerController 순회
    for (FConstPlayerControllerIterator ArrPC = World->GetPlayerControllerIterator(); ArrPC; ++ArrPC)
    {
        APlayerController* PC = ArrPC->Get();
        if (PC == nullptr)
        {
            continue;
        }

        // 만약 사용자 지정 컨트롤러라면 캐스트
        AGASWorldPlayerController* MyPC = Cast<AGASWorldPlayerController>(PC);
        if (MyPC)
        {
            // PC가 소유한 Pawn(캐릭터)을 가져와서 닉네임 설정
            APawn* Pawn = MyPC->GetPawn();
            AGASPlayerCharacter* MyChar = Cast<AGASPlayerCharacter>(Pawn);
            if (MyChar != nullptr)
            {
                // 예: 캐릭터에 저장된 ReplicatedNickname을 다시 세팅
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

    // 폰 위치 설정하기.
    Pawn->SetActorLocation(FindVillagePlayerStart()->GetActorLocation());
    Pawn->SetActorRotation(FindVillagePlayerStart()->GetActorRotation());

    Pawn->bIsDead = false;
    Pawn->bIsRespawned = true;

    //  AS 상태 변경
	if (AGASBasePlayerState* PS = Controller->GetPlayerState<AGASBasePlayerState>())
	{
		if (UGASCharacterAS* CharAS = PS->GetCharacterAttributeSet())
		{
			// 체력을 최대 체력으로 만들어준다.
			CharAS->SetHealth(CharAS->GetMaxHealth());
		}
        if (UAbilitySystemComponent* ASC = PS->GetPlayerCharacterGASC())
        {
            ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Dead.Player"));
        }
	}

    // 캐릭터 입력 허용
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {

        // 카메라 갱신
        PC->SetViewTargetWithBlend(Pawn, 0.f);

        // Possess 재확인
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
