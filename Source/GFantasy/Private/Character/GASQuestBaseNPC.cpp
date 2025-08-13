// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Character/GASQuestBaseNPC.h"
#include "Components/SphereComponent.h"
#include "Quest/QuestManagerComponent.h"
#include "Character/GASPlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Controller/GASWorldPlayerController.h"
#include "Components/TextRenderComponent.h"

AGASQuestBaseNPC::AGASQuestBaseNPC()
{
	bReplicates = true;
	SetReplicateMovement(true);
}

void AGASQuestBaseNPC::BeginPlay()
{
	Super::BeginPlay();
}

void AGASQuestBaseNPC::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);
}

void AGASQuestBaseNPC::ShowInteractionWidget(AActor* Interactor)
{
    Super::ShowInteractionWidget(Interactor);
}

FString AGASQuestBaseNPC::GetNameFromTag(const FString& FullTag) const
{
    int32 LastDotIndex = INDEX_NONE;
    if (FullTag.FindLastChar(TEXT('.'), LastDotIndex))
    {
        return FullTag.Mid(LastDotIndex + 1);
    }
    return FullTag;
}