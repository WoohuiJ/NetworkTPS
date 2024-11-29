// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStateUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UPlayerStateUI : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(class ANetPlayerState* PS);
	void UpdateName(FString Name);
	void UpdateScore(int32 Score);
	UFUNCTION()
	void OnUpdateScore();
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Name;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Score;

	UPROPERTY()
	class ANetPlayerState* PlayerState;
};