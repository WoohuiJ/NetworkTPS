// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NetPlayerState.generated.h"

/**
 * 
 */

DECLARE_DELEGATE(FUpdateScore);
UCLASS()
class NETTPS_API ANetPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ANetPlayerState(const FObjectInitializer& ObjectInitializer);

	virtual void OnRep_Score() override;

	FUpdateScore OnUpdateScore;

	UFUNCTION(Server, Reliable)
	void ServerRPC_SendChat(const FString& Chat);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SendChat(const FString& Chat);
};