// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Mannequin.generated.h"

class USkeletalMeshComponent;
class UCameraComponent;
class USpringArmComponent;
class UAnimMontage;
class USoundBase;


class AWeaponBase;
class ANadeActor;
class AMeleeActor;

UCLASS()
class S05_TESTINGGROUNDS_API AMannequin : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMannequin();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void UnPossessed() override;

	UFUNCTION(BlueprintCallable)
	void PullTrigger();

	void Pickup(AActor* PickedActor);

	void SwitchCamPosition();

	void SetCanPickup(bool boolcanpickup);

	void SetNadeThrown(UAnimMontage* NadeBasePose);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetWeapons(AWeaponBase*& CurrentWeaponOut, AWeaponBase*& PrimaryWeaponOut, AWeaponBase*& SecondaryWeaponOut, ANadeActor*& NadeOut, AMeleeActor*& MeleeOut);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetWeaponCombat();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetErrorMessage(bool& ShouldErrorOut, FString& ErrorMessageOut);

	bool GetIsFirstPerson();

	UCameraComponent* GetUsedCamera();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurrentMoney = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AmountOfNade = 3.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool NadeInHand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MaxHealth;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult & Hit) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsFirstPerson;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PrimaryInHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool SecondaryInHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool MeleeInHand;
	
	UPROPERTY(VisibleAnywhere)
	bool WeaponCombat;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToJump;
	UPROPERTY(BlueprintReadOnly)
	bool bWantToCrouch;
	UPROPERTY(BlueprintReadOnly)
	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool ShouldError;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ErrorMessage;
	FTimerHandle ErrorHandle;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "SpawnFromClass"))
	TSubclassOf<AWeaponBase> GunActor;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ANadeActor> NadeActor;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMeleeActor> MeleeActor;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* AmmoPickupSound;
	
	UPROPERTY(EditDefaultsOnly)
	USoundBase* HealthPickupSound;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* FP_ArmMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FP_Camera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* Spring_Arm;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TP_Camera;


private:
	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginJump();
	void EndJump();

	void BeginCrouch();
	void EndCrouch();

	void BeginZoom();
	void EndZoom();

	void TryPickup();
	void SetAndAttach(AWeaponBase* Weapon);

	void AttachToHand(AWeaponBase* Weapon);
	void AttachToBack(AWeaponBase* Weapon);

	void AttachNadeToHand(AActor* ActorToAttach);
	void AttachNadeToBack(AActor* ActorToAttach);

	void StartFire();
	void EndFire();

	void Drop();
	void DropSec();

	void ToggleCam();
	void Reload();

	void ResetError();

	void SpawnAndAttachWeapon(UClass* SpawnClass);
	void SpawnAndAttach();

	void PrimaryEquip();
	void SecondaryEquip();
	void MeleeEquip();
	void NadeEquip();

	UClass* SpawningClass;
	AWeaponBase* SpawnedWeapon;

	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLength = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLengthTP = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float ThrowMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float ZoomedFOV;
	UPROPERTY(EditDefaultsOnly)
	float ZoomInterpSpeed;
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly)
	bool SpawnFromClass;

	UPROPERTY(EditDefaultsOnly)
	bool PickupAttachDirect;

	bool CanPickup = true;

	bool canFire = true;
	bool isFiring;

	

	UPROPERTY(VisibleAnywhere)
	ANadeActor* Nade;
	UPROPERTY(VisibleAnywhere )
	AMeleeActor* Melee;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* CurrentWeapon = nullptr;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* PrimaryWeapon = nullptr;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* SecondaryWeapon = nullptr;

};
