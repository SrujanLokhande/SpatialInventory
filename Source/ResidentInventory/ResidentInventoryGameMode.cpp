// Copyright Epic Games, Inc. All Rights Reserved.

#include "ResidentInventoryGameMode.h"
#include "ResidentInventoryCharacter.h"
#include "UObject/ConstructorHelpers.h"

AResidentInventoryGameMode::AResidentInventoryGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
