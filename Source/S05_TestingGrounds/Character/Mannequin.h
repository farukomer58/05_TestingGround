// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Mannequin.generated.h"

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

	void TriggerTimer();
	void StopTriggerTimer();
	void StartTriggerTimer();

	void Pickup(AActor* PickedActor);

	void SwitchCamPosition();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	bool IsFirstPerson;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FP_Camera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* Spring_Arm;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TP_Camera;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "SpawnFromClass"))
	TSubclassOf<class AWeaponBase> GunActor;

	bool CanPickup = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* FP_ArmMesh;

private:

	void TryPickup();

	void StartFire();
	void EndFire();


	void Drop();
	void DropSec();

	void ToggleCam();

	void SpawnAndAttachWeapon(UClass* SpawnClass);
	void SpawnAndAttach();

	UClass* SpawningClass;

	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLength = 300.f;
	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLengthTP = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float ThrowMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly)
	bool SpawnFromClass;

	bool canFire = true;
	bool isFiring;

	bool canHandle = true;

	FTimerHandle UnusedHandle;

	UPROPERTY(EditDefaultsOnly)
	bool PickupAttachDirect;

	bool PrimaryInHand;
	bool SecondaryInHand;

	void SetAndAttach(AWeaponBase* Weapon);

	void AttachToHand(AWeaponBase* Weapon);
	void AttachToBack(AWeaponBase* Weapon);

	UPROPERTY(VisibleAnywhere)
	AWeaponBase* CurrentWeapon = nullptr;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* PrimaryWeapon = nullptr;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* SecondaryWeapon = nullptr;
};
