// Fill out your copyright notice in the Description page of Project Settings.
#include "GameMode/BaseGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"


UCharacterClassInfo* ABaseGameMode::GetCharacterClassDefaultInfo() const
{
    return ClassDefaults;
}

AActor* ABaseGameMode::FindVillagePlayerStart() const
{
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        // 태그가 있는 경우 특정 PlayerStart만 찾기
        
        // 현재는 하나밖에 없음.
        return *It;
    }

    return nullptr;
}
