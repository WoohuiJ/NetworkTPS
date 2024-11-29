// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateUI.h"
#include "NetPlayerState.h"
#include "Components/TextBlock.h"

void UPlayerStateUI::Init(class ANetPlayerState* PS)
{
	PlayerState = PS;
	UpdateName(PS->GetPlayerName());
	UpdateScore(PS->GetScore());

	PS->OnUpdateScore.BindUObject(this, &UPlayerStateUI::OnUpdateScore);
}

void UPlayerStateUI::UpdateName(FString Name)
{
	Text_Name->SetText(FText::FromString(Name));
}

void UPlayerStateUI::UpdateScore(int32 Score)
{
	Text_Score->SetText(FText::AsNumber(Score));
}

void UPlayerStateUI::OnUpdateScore()
{
	UpdateScore(PlayerState->GetScore());
}