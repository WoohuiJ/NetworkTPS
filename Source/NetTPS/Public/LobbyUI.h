// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void CreateSession();
	UFUNCTION()
	void FindSession();
	UFUNCTION()
	void OnValueChanged(float Value);
	UFUNCTION()
	void GoCreate();
	UFUNCTION()
	void GoFind();
	UFUNCTION()
	void OnAddSession(int32 Index, FString Info);
	UFUNCTION()
	void OnFindComplete(bool bIsComplete);
public:
	UPROPERTY()
	class UNetGameInstance* GI;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionItem> SessionItemClass;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Create;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Find;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Edit_DisplayName;
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider_PlayerCount;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_PlayerCount;
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* Scroll_SessionList;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Go_Create;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Go_Find;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
};
