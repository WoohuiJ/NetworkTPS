// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthBar.h"
#include "InputActionValue.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MainUI.h"
#include "NetTPSGameMode.h"
#include "Pistol.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTPSCharacter

ANetTPSCharacter::ANetTPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0,0,60));
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CompGun = CreateDefaultSubobject<USceneComponent>(TEXT("Gun"));
	CompGun->SetupAttachment(GetMesh(), TEXT("GunPosition"));
	CompGun->SetRelativeLocation(FVector(-7.144f, 3.68f, 4.136f));
	CompGun->SetRelativeRotation(FRotator(3.4f, 75.699f, 6.642f));

	CompHP = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP"));
	CompHP->SetupAttachment(RootComponent);
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ANetTPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	FString isServer = HasAuthority() ? TEXT("서버") : TEXT("클라");
	UE_LOG(LogTemp, Warning, TEXT("%s - BeginPlay : %s"), *isServer, *GetActorNameOrLabel());
	OriginCamPos = CameraBoom->GetRelativeLocation();

	if (HasAuthority())
	{
		ANetTPSGameMode* GM = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->AddPlayer(this);
		}
		if (IsLocallyControlled())
		{
			bCanMakeCube = true;
		}
	}
}

void ANetTPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector pos = FMath::Lerp(CameraBoom->GetRelativeLocation(), OriginCamPos, DeltaTime * 10);
	CameraBoom->SetRelativeLocation(pos);

	PrintNetLog();
	BillboardHP();
}

void ANetTPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetTPSCharacter, bCanMakeCube);
	DOREPLIFETIME(ANetTPSCharacter, OwnedPistol);
}

//
void ANetTPSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogTemp, Warning, TEXT("PossessedBy : %s"), *GetActorNameOrLabel());
	ClientRPC_Init();
}

void ANetTPSCharacter::PrintNetLog()
{
	FString connStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	FString role = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	FString isMine = IsLocallyControlled() ? TEXT("내 것") : TEXT("남의 것");
	FString logStr = FString::Printf(TEXT("Connection : %s\nOwner : %s\nRole : %s\nMine : %s"),
		*connStr,
		*ownerStr,
		*role,
		*isMine);
	
	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Look);

		// Take Pistol
		EnhancedInputComponent->BindAction(TakeAction, ETriggerEvent::Started, this, &ANetTPSCharacter::TakePistol);

		// Fire Pistol
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Fire);

		// Reload Pistol
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Reload);
		
	//	EnhancedInputComponent->BindAction(MakeCubeAction, ETriggerEvent::Started, this, &ANetTPSCharacter::MakeCube);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANetTPSCharacter::ServerRPC_TakePistol_Implementation()
{
	// if no gun is possessed, grab gun in range
	if (bHasPistol == false)
	{
		TArray<AActor*> AllActors;
		TArray<APistol*> PistolActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APistol::StaticClass(), AllActors);
		for(auto Actor : AllActors)
		{
			PistolActors.Add(Cast<APistol>(Actor));
		}
		APistol* ClosestPistol = nullptr;
		float ClosestDist = std::numeric_limits<float>::max();
		for (auto Pistol : PistolActors)
		{
			if (Pistol->GetOwner() == nullptr)
			{
				
				float dist = FVector::Dist(Pistol->GetActorLocation(), GetActorLocation());
				if (dist < distanceToGun)
				{
					if(ClosestDist > dist)
					{
						ClosestDist = dist;
						ClosestPistol = Pistol;
					}
				}
			}
			if(ClosestPistol)
			{
				OwnedPistol = ClosestPistol;
				OwnedPistol->SetOwner(this);
				AttachPistol();
			}
		}
	}
	else
	{
		APistol* Pistol = OwnedPistol;
		OwnedPistol->SetOwner(nullptr);
		OwnedPistol = nullptr;
		MulticastRPC_DetachPistol(Pistol);
	}
}

void ANetTPSCharacter::TakePistol()
{
	ServerRPC_TakePistol();
}

void ANetTPSCharacter::AttachPistol()
{
	if(OwnedPistol == nullptr) return;	
	UStaticMeshComponent* comp = OwnedPistol->FindComponentByClass<UStaticMeshComponent>();
	if (comp)
	{
		comp->SetSimulatePhysics(false);
		OwnedPistol->AttachToComponent(CompGun, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		OwnedPistol->SetOwner(this);
		bHasPistol = true;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		if(IsLocallyControlled())
		{
			CameraBoom->TargetArmLength = 150;
			OriginCamPos = FVector(0, 40, 60);
			InitBulletUI();
		}
	}
}

void ANetTPSCharacter::MulticastRPC_DetachPistol_Implementation(APistol* Pistol)
{
	DetachPistol(Pistol);
}

void ANetTPSCharacter::DetachPistol(APistol* Pistol)
{
	UStaticMeshComponent* comp = Pistol->FindComponentByClass<UStaticMeshComponent>();
	if (comp)
	{
		comp->SetSimulatePhysics(true);
		
		Pistol->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		Pistol->SetOwner(nullptr);
		bHasPistol = false;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		
		if(IsLocallyControlled())
		{
			CameraBoom->TargetArmLength = 300;
			OriginCamPos = FVector(0, 0, 60);
			MainUI->PopAllBullet();
			MainUI->ShowCrossHair(false);
		}
	}
}

void ANetTPSCharacter::MulticastRPC_Fire_Implementation(bool bHit, FHitResult Hit)
{
	if (bHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunEffect, Hit.Location, FRotator::ZeroRotator, true);

		ANetTPSCharacter* Player = Cast<ANetTPSCharacter>(Hit.GetActor());
		if(Player)
		{
			Player->DamageProcess(OwnedPistol->WeaponDamage);
		}
	}
	PlayAnimMontage(FireMontage, 2, TEXT("Fire"));
	OwnedPistol->CurrentBulletCount--;
	if(OwnedPistol->CurrentBulletCount <= 0)
	{
		Reload();
	}
	if(IsLocallyControlled())
	{
		MainUI->PopBullet(OwnedPistol->CurrentBulletCount);
	}
}

void ANetTPSCharacter::ServerRPC_Fire_Implementation(bool bHit, FHitResult Hit)
{
	MulticastRPC_Fire(bHit, Hit);
}

void ANetTPSCharacter::Fire()
{
	// if no gun is possesed, return
	if(!bHasPistol) return;
	
	if(OwnedPistol->CurrentBulletCount <= 0)return;

	if(bIsReloading) return;
	
	// Find Collision point by LineTrace
	FVector startPos = FollowCamera->GetComponentLocation();
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 100000;
	
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	FHitResult Hit;
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, startPos, endPos, ECollisionChannel::ECC_Visibility, params);

	ServerRPC_Fire(bHit, Hit);
}

void ANetTPSCharacter::MulticastRPC_Reload_Implementation()
{
	PlayAnimMontage(FireMontage, 1, TEXT("Reload"));
}

void ANetTPSCharacter::ServerRPC_Reload_Implementation()
{
	if(!bHasPistol) return;

	if(OwnedPistol->CurrentBulletCount == OwnedPistol->MaxBulletCount)return;

	if(bIsReloading) return;
	
	bIsReloading = true;
	
	MulticastRPC_Reload();
}

void ANetTPSCharacter::Reload()
{
	ServerRPC_Reload();
}

void ANetTPSCharacter::MulticastRPC_MakeCube_Implementation(FVector Pos, FRotator Rot)
{
	FActorSpawnParameters SpawnParameters;
	GetWorld()->SpawnActor<AActor>(CubeClass, Pos, Rot, SpawnParameters);
}

void ANetTPSCharacter::ServerRPC_MakeCube_Implementation()
{
	ANetTPSGameMode* GM = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	GM->ChangeTurn();
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100;
	FRotator SpawnRotation = GetActorRotation();
	MulticastRPC_MakeCube(SpawnLocation, SpawnRotation);
}

void ANetTPSCharacter::MakeCube()
{
	if(!bCanMakeCube)return;
	ServerRPC_MakeCube();
}

void ANetTPSCharacter::ReloadFinish()
{
	OwnedPistol->Reload();
	InitBulletUI();
	bIsReloading = false;
}

void ANetTPSCharacter::BillboardHP()
{
	//Find Camera
	AActor* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	//Find Camera Forward Vector
	FVector Forward = -Camera->GetActorForwardVector();
	//Find Camera Up Vector
	FVector Up = Camera->GetActorUpVector();
	//Get the rotation of HP bar using two vectors
	FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, Up);
	CompHP->SetWorldRotation(Rot);
}


void ANetTPSCharacter::InitBulletUI()
{
	if(IsLocallyControlled() == false) return;
	//crosshair show
	MainUI->ShowCrossHair(true);
	
	MainUI->PopAllBullet();
	for(int i = 0 ; i < OwnedPistol->CurrentBulletCount; i++)
	{
		MainUI->AddBullet();
	}
}

void ANetTPSCharacter::InitMainUIWidget()
{
	if(IsLocallyControlled() == false) return;
	MainUI = Cast<UMainUI>(CreateWidget(GetWorld(), MainUIClass));
	MainUI->AddToViewport();
	CompHP->SetVisibility(false);
}

void ANetTPSCharacter::ClientRPC_Init_Implementation()
{
	InitMainUIWidget();
}

void ANetTPSCharacter::DamageProcess(float Damage)
{
	UHealthBar* HPBar = nullptr;
	if(IsLocallyControlled())
	{
		HPBar = MainUI->HealthBar;
	}
	else
	{
		HPBar = Cast<UHealthBar>(CompHP->GetWidget());
	}
	float CurrentHP = HPBar->UpdateHPBar(Damage);
	if(CurrentHP <= 0)
	{
		bIsDead = true;
		GetCharacterMovement()->DisableMovement();

		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		
		if(bHasPistol && IsLocallyControlled())
		{
			TakePistol();
		}
	}
}

void ANetTPSCharacter::DeathProcess()
{
	if(IsLocallyControlled() == false) return;
	FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0,0,0,1);
	APlayerController* PC = Cast<APlayerController>(Controller);
	PC->SetShowMouseCursor(true);
	MainUI->ShowCrossHair(true);
	MainUI->ShowBtnRetry(true);
}