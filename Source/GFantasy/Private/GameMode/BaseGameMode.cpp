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
        // �±װ� �ִ� ��� Ư�� PlayerStart�� ã��
        
        // ����� �ϳ��ۿ� ����.
        return *It;
    }

    return nullptr;
}
