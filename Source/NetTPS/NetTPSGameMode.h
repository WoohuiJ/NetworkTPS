// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetTPSGameMode.generated.h"

UCLASS(minimalapi)
class ANetTPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANetTPSGameMode();

public:
	UPROPERTY(VisibleAnywhere)
	TArray<class ANetTPSCharacter*> AllPlayers;
	
	int32 TurnIndex = 0;
public:
	void AddPlayer(class ANetTPSCharacter* Player);
	void ChangeTurn();
};