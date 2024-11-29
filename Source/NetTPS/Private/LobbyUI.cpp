// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyUI.h"

#include "NetGameInstance.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "SessionItem.h"

void ULobbyUI::NativeConstruct()
{
	Super::NativeConstruct();
	GI = Cast<UNetGameInstance>( GetGameInstance());
	GI->OnAddSession.BindUObject(this, &ULobbyUI::OnAddSession);
	GI->OnFindComplete.BindUObject(this, &ULobbyUI::OnFindComplete);
	Btn_Create->OnClicked.AddDynamic(this, &ULobbyUI::CreateSession);
	Btn_Find->OnClicked.AddDynamic(this, &ULobbyUI::FindSession);
	Btn_Go_Create->OnClicked.AddDynamic(this, &ULobbyUI::GoCreate);
	Btn_Go_Find->OnClicked.AddDynamic(this, &ULobbyUI::GoFind);
	Slider_PlayerCount->OnValueChanged.AddDynamic(this, &ULobbyUI::OnValueChanged);
}

void ULobbyUI::CreateSession()
{
	if(Edit_DisplayName->GetText().IsEmpty())return;
	FString DisplayName = Edit_DisplayName->GetText().ToString();
	int32 PlayerCount = Slider_PlayerCount->GetValue();
	GI->CreateMySession(DisplayName,PlayerCount);
}

void ULobbyUI::FindSession()
{
	Scroll_SessionList->ClearChildren();
	Btn_Find->SetIsEnabled(false);
	GI->FindOtherSession();
}

void ULobbyUI::OnValueChanged(float Value)
{
	Text_PlayerCount->SetText(FText::AsNumber(Value));
}

void ULobbyUI::GoCreate()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULobbyUI::GoFind()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);
	FindSession();
}

void ULobbyUI::OnAddSession(int32 Index, FString Info)
{
	USessionItem* Item = CreateWidget<USessionItem>(GetWorld(), SessionItemClass);
	Scroll_SessionList->AddChild(Item);
	Item->SetInfo(Index, Info);
}

void ULobbyUI::OnFindComplete(bool bIsComplete)
{
	Btn_Find->SetIsEnabled(bIsComplete);
}
