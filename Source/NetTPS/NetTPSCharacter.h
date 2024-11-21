// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetTPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* TakeAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ReloadAction;
public:
	ANetTPSCharacter();
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerRPC_TakePistol();
	void TakePistol();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_AttachPistol(class APistol* Pistol);
	void AttachPistol(class APistol* Pistol);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DetachPistol(APistol* Pistol);
	void DetachPistol(APistol* Pistol);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Fire(bool bHit, FHitResult Hit);
	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire(bool bHit, FHitResult Hit);
	void Fire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Reload();
	UFUNCTION(Server, Reliable)
	void ServerRPC_Reload();
	void Reload();
	
	void InitMainUIWidget();

public:
	void ReloadFinish();

	void InitBulletUI();

	void BillboardHP();
public:
	UPROPERTY(EditAnywhere)
	USceneComponent* CompGun;
	
	bool bHasPistol = false;

	UPROPERTY(EditAnywhere)
	float distanceToGun = 200;

	UPROPERTY(EditAnywhere)
	FVector OriginCamPos;

	UPROPERTY(EditAnywhere)
	class APistol* OwnedPistol = nullptr;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* GunEffect;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMainUI> MainUIClass;

	UPROPERTY(EditAnywhere)
	UMainUI* MainUI;

	UPROPERTY(EditAnywhere)
	float MaxBulletCount = 10;

	float CurrentBulletCount = 0;

	bool bIsReloading = false;

	//HealthBar UI Component
	UPROPERTY(EditAnywhere)
	class UWidgetComponent* CompHP;
	
	void DamageProcess(float Damage);
	
	bool bIsDead= false;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	
	virtual void Tick(float DeltaTime) override;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void PrintNetLog();
};

