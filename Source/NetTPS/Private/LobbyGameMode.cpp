// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyUI.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	LobbyUI = CreateWidget<ULobbyUI>(GetWorld(), LobbyUIClass);
	LobbyUI->AddToViewport();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	PC->SetShowMouseCursor(true);
}