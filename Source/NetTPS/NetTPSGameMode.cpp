// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSGameMode.h"
#include "NetTPSCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
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

	int32 SpawnPosCount = 4;
	float SpawnDegree = 360.f / SpawnPosCount;

	//PlayerStart
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());

	//
	float radian = FMath::DegreesToRadians(SpawnDegree);
	FVector Pos;
	FVector Dir = FVector(
		FMath::Sin(PosIndex * radian),
		FMath::Cos(PosIndex * radian),
		PlayerStart->GetActorLocation().Z
		);
	Pos = PlayerStart->GetActorLocation() + Dir * 200;
	Player->SetActorLocation(Pos);
	PosIndex =(PosIndex + 1) % SpawnPosCount;
	//
	// 	Player->SetActorRotation(FRotator(0, PosIndex*SpawnDegree, 0));
	// 	FVector Pos = PlayerStart->GetActorLocation() + PlayerStart->GetActorForwardVector() * 200;
	// 	Player->SetActorLocation(Pos);
	// 	PosIndex = (PosIndex +1) % SpawnPosCount;
	// 	
	// 	for(int32 i = 0; i < SpawnPosCount; i++)
	// 	{
	// 		// PlayerStart->SetActorRotation(FRotator(0, i*SpawnDegree, 0));
	// 		// FVector Pos = PlayerStart->GetActorLocation() + PlayerStart->GetActorForwardVector() * 200;
	// 		//
	// 	}
	// }
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