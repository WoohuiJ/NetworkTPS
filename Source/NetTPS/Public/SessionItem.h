// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionItem.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API USessionItem : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void Join();

	void SetInfo(int32 Index, FString Info);
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Join;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_SessionName;

	int32 SelectIndex;
};
