// Fill out your copyright notice in the Description page of Project Settings.


#include "GameUI.h"

#include "NetGameState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateUI.h"
#include "NetPlayerState.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"


class FPlayerStateSort
{
public:
	bool operator() (const APlayerState& a, const APlayerState& b) const
	{
		return (a.GetPlayerId() < b.GetPlayerId());
	}
};
void UGameUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//Get GameState
	AGameStateBase* GameState = GetWorld()->GetGameState();
	//Print Player Name to TextBlock From Obtained GameState
	FString AllUserName;
	FString PlayerName;
	int32 PlayerID;
	int32 Score;
	APlayerState* PS;

	GameState->PlayerArray.Sort(FPlayerStateSort());
	
	for(int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		PS = GameState->PlayerArray[i];
		PlayerName = PS->GetPlayerName();
		PlayerID = PS->GetPlayerId();
		Score = PS->GetScore();
		
		AllUserName.Append(FString::Printf(TEXT("[%d] %s : %d\n"),PlayerID, *PlayerName, Score));
	}
	Text_PlayerState->SetText(FText::FromString(AllUserName));
}

void UGameUI::AddPlayerStateUI(APlayerState* PS)
{
	if(PS == nullptr)return;
	UPlayerStateUI* PSUI = CreateWidget<UPlayerStateUI>(GetWorld(), PlayerStateUIClass);
	PSUI->Init(Cast<ANetPlayerState>(PS));
	VBox_PlayerState->AddChild(PSUI);
	UVerticalBoxSlot* _Slot = Cast<UVerticalBoxSlot>(PSUI->Slot);
	_Slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
	_Slot->SetPadding(FMargin(0,0,20,0));
}

void UGameUI::AddChat(FString Chat)
{
	
}

void UGameUI::OnTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	
}

// FEventReply UGameUI::OnPointerEvent(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
// {
// 	//Cast<ANetGameState>(GetWorld()->GetGameState())->ShowCursor(false);
//
// 	return FEventReply();
// }
