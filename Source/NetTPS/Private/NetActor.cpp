// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "NetTPS/NetTPSCharacter.h"

// Sets default values
ANetActor::ANetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CompMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CompMesh"));
	RootComponent = CompMesh;
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FindOwner();
	PrintNetLog();
	Rotate();
}

void ANetActor::PrintNetLog()
{
	FString connStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	FString role = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString logStr = FString::Printf(TEXT("Connection : %s\nOwner : %s\nRole : %s\n"),
		*connStr,
		*ownerStr,
		*role
		);
	
	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);
}

void ANetActor::FindOwner()
{
	if(HasAuthority() == false) return;
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetTPSCharacter::StaticClass(), AllPlayers);
	AActor* _Owner = nullptr;
	float ClosestDistance = SearchDistance;
	for(int32 i = 0; i < AllPlayers.Num(); i++)
	{
		float dist = FVector::Distance(GetActorLocation(), AllPlayers[i]->GetActorLocation());
		if(dist < ClosestDistance)
		{
			ClosestDistance = dist;
			_Owner = AllPlayers[i];
		}
	}
	if(GetOwner() != _Owner)
	{
		SetOwner(_Owner);
	}
}

void ANetActor::Rotate()
{
	if(HasAuthority() == true)
	{
		AddActorWorldRotation(FRotator(0, 50 * GetWorld()->DeltaTimeSeconds, 0));
		RotYaw = GetActorRotation().Yaw;
	}
}

void ANetActor::OnRep_RotYaw()
{
	FRotator Rot = GetActorRotation();
	Rot.Yaw = RotYaw;
	SetActorRotation(Rot);
}

void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Register the variable that needs to be replicated
	DOREPLIFETIME(ANetActor, RotYaw);
}
