// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"

#include "GameUI.h"
#include "NetGameState.h"

ANetPlayerState::ANetPlayerState(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	//NetUpdateFrequency = 100;
}

void ANetPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	OnUpdateScore.ExecuteIfBound();
}

void ANetPlayerState::MulticastRPC_SendChat_Implementation(const FString& Chat)
{
	ANetGameState* GameState = Cast<ANetGameState>(GetWorld()->GetGameState());
	GameState->GameUI->AddChat(Chat);
}

void ANetPlayerState::ServerRPC_SendChat_Implementation(const FString& Chat)
{
	MulticastRPC_SendChat(Chat);
}
