// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionItem.h"

#include "NetGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USessionItem::NativeConstruct()
{
	Super::NativeConstruct();
	Btn_Join->OnClicked.AddDynamic(this, &USessionItem::Join);
}

void USessionItem::Join()
{
	UNetGameInstance* GI = Cast<UNetGameInstance>(GetGameInstance());
	GI->JoinOtherSession(SelectIndex);
}

void USessionItem::SetInfo(int32 Index, FString Info)
{
	SelectIndex = Index;
	Text_SessionName->SetText(FText::FromString(Info));
}