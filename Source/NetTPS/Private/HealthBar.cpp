// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"
#include "Components/ProgressBar.h"


float UHealthBar::UpdateHPBar(float Damage)
{
	CurrentHP -= Damage;

	float Percent = CurrentHP / MaxHP;
	
	HPBar->SetPercent(Percent);

	return CurrentHP;
}

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentHP = MaxHP;
}