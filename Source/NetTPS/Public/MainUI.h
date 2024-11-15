// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UImage* CrossHairImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UHorizontalBox* BulletPanel;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> BulletUIClass; 
public:
	void ShowCrossHair(bool bIsVisible);

	void AddBullet();

	void PopBullet(int32 index);
protected:
	virtual void NativeConstruct() override;
};