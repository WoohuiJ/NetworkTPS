// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"

ANetPlayerState::ANetPlayerState(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	//NetUpdateFrequency = 100;
}

void ANetPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	OnUpdateScore.ExecuteIfBound();
}
