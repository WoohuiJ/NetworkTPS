// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSGameMode.h"
#include "NetTPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetTPSGameMode::ANetTPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ANetTPSGameMode::AddPlayer(class ANetTPSCharacter* Player)
{
	AllPlayers.Add(Player);
}

void ANetTPSGameMode::ChangeTurn()
{
	AllPlayers[TurnIndex]->bCanMakeCube = false;
	TurnIndex++;

	if (TurnIndex >= AllPlayers.Num())
	{
		TurnIndex = 0;
	}
	AllPlayers[TurnIndex]->bCanMakeCube = true;
}