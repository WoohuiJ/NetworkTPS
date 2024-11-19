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
#include "Pistol.h"
#include "Components/WidgetComponent.h"

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
	InitMainUIWidget();
	OriginCamPos = CameraBoom->GetRelativeLocation();
}

void ANetTPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector pos = FMath::Lerp(CameraBoom->GetRelativeLocation(), OriginCamPos, DeltaTime * 10);
	CameraBoom->SetRelativeLocation(pos);

	PrintNetLog();
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

void ANetTPSCharacter::TakePistol()
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
			AttachPistol(ClosestPistol);
		}
	}
	else
	{
		DetachPistol();
	}
}

void ANetTPSCharacter::AttachPistol(APistol* Pistol)
{
	if (!Pistol) return;

	UStaticMeshComponent* comp = Pistol->FindComponentByClass<UStaticMeshComponent>();
	if (comp)
	{
		comp->SetSimulatePhysics(false);
		Pistol->AttachToComponent(CompGun, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Pistol->SetOwner(this);
		bHasPistol = true;
		OwnedPistol = Pistol;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		CameraBoom->TargetArmLength = 150;
		OriginCamPos = FVector(0, 40, 60);

		//crosshair show
		MainUI->ShowCrossHair(true);
		InitBulletUI();
	}
}

void ANetTPSCharacter::DetachPistol()
{
	if (!OwnedPistol) return;

	UStaticMeshComponent* comp = OwnedPistol->FindComponentByClass<UStaticMeshComponent>();
	if (comp)
	{
		comp->SetSimulatePhysics(true);
		
		OwnedPistol->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		OwnedPistol->SetOwner(nullptr);
		bHasPistol = false;
		OwnedPistol = nullptr;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		CameraBoom->TargetArmLength = 300;
		OriginCamPos = FVector(0, 0, 60);

		MainUI->PopAllBullet();
		MainUI->ShowCrossHair(false);
		
	}
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
	if(bHit)
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
	MainUI->PopBullet(OwnedPistol->CurrentBulletCount);
}

void ANetTPSCharacter::Reload()
{
	if(!bHasPistol) return;

	if(OwnedPistol->CurrentBulletCount == OwnedPistol->MaxBulletCount)return;

	if(bIsReloading) return;
	bIsReloading = true;
	
	PlayAnimMontage(FireMontage, 1, TEXT("Reload"));
}

void ANetTPSCharacter::ReloadFinish()
{
	OwnedPistol->Reload();
	InitBulletUI();
	bIsReloading = false;
}

void ANetTPSCharacter::InitBulletUI()
{
	MainUI->PopAllBullet();
	for(int i = 0 ; i < OwnedPistol->CurrentBulletCount; i++)
	{
		MainUI->AddBullet();
	}
}

void ANetTPSCharacter::InitMainUIWidget()
{
	MainUI = Cast<UMainUI>(CreateWidget(GetWorld(), MainUIClass));
	MainUI->AddToViewport();
	// CurrentBulletCount = MaxBulletCount;
	// for(int i = 0; i < CurrentBulletCount; i++)
	// {
	// 	MainUI->AddBullet();
	// }
}

void ANetTPSCharacter::DamageProcess(float Damage)
{
	UHealthBar* HPBar = Cast<UHealthBar>(CompHP->GetWidget());
	float CurrentHP = HPBar->UpdateHPBar(Damage);
	if(CurrentHP <= 0)
	{
		bIsDead = true;
	}
}