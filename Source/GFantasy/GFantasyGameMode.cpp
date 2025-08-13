// Copyright Epic Games, Inc. All Rights Reserved.

#include "GFantasyGameMode.h"
#include "GFantasyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGFantasyGameMode::AGFantasyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
