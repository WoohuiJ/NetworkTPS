// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_PlayerState;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VBox_PlayerState;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerStateUI> PlayerStateUIClass;
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void AddPlayerStateUI(class APlayerState* PS);
	
};