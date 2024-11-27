// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"

#include "GameFramework/GameModeBase.h"
#include "GameFramework/SpectatorPawn.h"

void ANetPlayerController::ServerRPC_ChangeToSpectator_Implementation()
{
	//Get the pawn in possession
	APawn* Current = GetPawn();
	//Unpossess the pawn in possession
	UnPossess();
	//create spectator pawn
	AGameModeBase* GM = GetWorld()->GetAuthGameMode();
	ASpectatorPawn* Spectator = GetWorld()->SpawnActor<ASpectatorPawn>(GM->SpectatorClass, Current->GetActorTransform());
	//possess spectator pawn
	Possess(Spectator);
	//destroy previous pawn
	Current->Destroy();
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ANetPlayerController::RespawnPlayer, 5, false);
}

void ANetPlayerController::RespawnPlayer()
{
	//Get the pawn in possession
	APawn* Current = GetPawn();
	//Unpossess the pawn in possession
	UnPossess();
	Current->Destroy();
	
	AGameModeBase* GM = GetWorld()->GetAuthGameMode();
	GM->RestartPlayer(this);
}