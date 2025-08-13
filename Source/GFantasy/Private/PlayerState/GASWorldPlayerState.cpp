// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/GASWorldPlayerState.h"
#include "GAS/AbilitySystemComponent/PlayerCharacterGASC.h"
#include "Controller/GASWorldPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/GASPlayerCharacter.h"
#include "Character/Customization/CustomizerComponent.h"

AGASWorldPlayerState::AGASWorldPlayerState()
{
	//ASC 珥덇린??	//PlayerASC = CreateDefaultSubobject<UPlayerCharacterGASC>(TEXT("PlayerASC"));
	//PlayerASC->SetIsReplicated(true);
	//PlayerASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	//ASC = PlayerASC;
	bReplicates = true;

	CustomizerComponent = CreateDefaultSubobject<UCustomizerComponent>("CustomizerComponent");
	CustomizerComponent->SetIsReplicated(true);
}

UCustomizerComponent* AGASWorldPlayerState::GetCustomizerComponent_Implementation()
{
	return CustomizerComponent;
}

void AGASWorldPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AGASWorldPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGASWorldPlayerState, PlayerNickname);

	DOREPLIFETIME(AGASWorldPlayerState, CustomizerComponent);
}

void AGASWorldPlayerState::SetPlayerNickName(const FString& InName)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Set PS Nickname : %s"), *InName));
	PlayerNickname = InName;
}

void AGASWorldPlayerState::OnRep_PlayerNickName()
{
    UE_LOG(LogTemp, Log, TEXT("PS : Nickname OnRep_PlayerNickName"))
    AGASWorldPlayerController* PC = Cast<AGASWorldPlayerController>(GetOwner());
    if (!PC)
    {
        // 혹시 PlayerState 소유자를 못 찾으면 월드 순회 방식으로
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            AGASWorldPlayerController* TryPC = Cast<AGASWorldPlayerController>(*It);
            if (TryPC && TryPC->PlayerState == this)
            {
                PC = TryPC;
                break;
            }
        }
    }

    if (PC)
    {
        PC->ClientSetNickname(PlayerNickname);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnRep_PlayerNickName: Could not find owning GASWorldPlayerController"));
    }
}


FString AGASWorldPlayerState::GetPlayerNickname() const
{
	return PlayerNickname;
}