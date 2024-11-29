// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NetGameState.generated.h"

class ANetPlayerState;
/**
 * 
 */
UCLASS()
class NETTPS_API ANetGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	class UGameUI* GetGameUI();
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameUI>GameUIClass;

	UPROPERTY(EditAnywhere)
	UGameUI* GameUI;
};