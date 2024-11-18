// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"

void UMainUI::ShowCrossHair(bool bIsVisible)
{
	if(bIsVisible)
	{
		CrossHairImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CrossHairImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainUI::AddBullet()
{
	UUserWidget* Bullet = CreateWidget(GetWorld(), BulletUIClass);
	BulletPanel->AddChild(Bullet);
}

void UMainUI::PopBullet(int32 index)
{
	BulletPanel->RemoveChildAt(index);
}

void UMainUI::PopAllBullet()
{
	int32 RemainingBullets = BulletPanel->GetChildrenCount();
	for(int32 i = RemainingBullets - 1; i >= 0; i--)
	{
		PopBullet(i);
	}
}

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();
	CrossHairImage = Cast<UImage>(GetWidgetFromName(TEXT("CrossHair")));
	ShowCrossHair(false);
}