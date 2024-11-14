// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"
#include "NetTPS/NetTPSCharacter.h"


void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	//get pawn
	player = Cast<ANetTPSCharacter>(TryGetPawnOwner());
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(player)
	{
		bHasPistol = player->bHasPistol;
		// dirV -> front, back
		dirV = FVector::DotProduct(player->GetActorForwardVector(), player->GetVelocity());
	
		// dirH -> left, right
		dirH = FVector::DotProduct(player->GetActorRightVector(), player->GetVelocity());
	}
}
