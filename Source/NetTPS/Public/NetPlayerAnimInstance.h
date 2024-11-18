// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bHasPistol = false;
	
	// dirH, dirV
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float dirH = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float dirV = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	bool bIsDead = false;
	
	UPROPERTY()
	class ANetTPSCharacter* player;

public:
	UFUNCTION()
	void AnimNotify_OnReloadFinish();
};
