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

	UPROPERTY(meta = (BindWidget))
	class UHealthBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Retry;
public:
	void ShowCrossHair(bool bIsVisible);

	void AddBullet();

	void PopBullet(int32 index);

	void PopAllBullet();

	void ShowBtnRetry(bool bIsVisible);

	UFUNCTION()
	void OnRetry();
	
protected:
	virtual void NativeConstruct() override;
};