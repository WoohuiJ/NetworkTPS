// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameState.h"
#include "GameUI.h"
void ANetGameState::BeginPlay()
{
	Super::BeginPlay();
}

class UGameUI* ANetGameState::GetGameUI()
{
	if(GameUI == nullptr)
	{
		GameUI = CreateWidget<UGameUI>(GetWorld(), GameUIClass);
		GameUI->AddToViewport();
	}
	return GameUI;
}
