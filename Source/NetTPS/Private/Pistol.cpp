// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"

// Sets default values
APistol::APistol()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void APistol::BeginPlay()
{
	Super::BeginPlay();
	CurrentBulletCount = MaxBulletCount;
}

// Called every frame
void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APistol::bIsMaxBulletCount()
{
	return CurrentBulletCount == MaxBulletCount; 
}

void APistol::Reload()
{
	CurrentBulletCount = MaxBulletCount;
}