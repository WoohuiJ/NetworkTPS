// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

UCLASS()
class NETTPS_API ANetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* CompMesh;

	void PrintNetLog();

	void FindOwner();

	void Rotate();

	UPROPERTY(ReplicatedUsing = OnRep_RotYaw)
	float RotYaw = 0;

	UFUNCTION()
	void OnRep_RotYaw();
	
	UPROPERTY(EditAnywhere)
	float SearchDistance = 200;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
