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
	
	UPROPERTY()
	TArray<class UPlayerStateUI*> allPlayerState;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* edit_chat;
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* scroll_chat;
	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<class UChatItem> chatItemFactory;

	UPROPERTY(meta = (BindWidget))
	class UBorder* emptyBorder;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void AddPlayerStateUI(class APlayerState* PS);

	void AddChat(FString Chat);
	
	UFUNCTION()
	void OnTextBoxCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	//
	// UFUNCTION()
	// FEventReply OnPointerEvent(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
};