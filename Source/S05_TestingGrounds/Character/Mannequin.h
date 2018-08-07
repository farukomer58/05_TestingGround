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

	void Pickup(AActor* PickedActor);

	void SwitchCamPosition();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	bool IsFirstPerson;

	bool CanPickup = true;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FP_Camera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* Spring_Arm;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TP_Camera;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "SpawnFromClass"))
	TSubclassOf<class AWeaponBase> GunActor;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ANadeActor> NadeActor;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AMeleeActor> MeleeActor;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetWeapons(AWeaponBase*& CurrentWeaponOut, AWeaponBase*& PrimaryWeaponOut, AWeaponBase*& SecondaryWeaponOut, ANadeActor*& NadeOut, AMeleeActor*& MeleeOut);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool PrimaryInHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool SecondaryInHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool MeleeInHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool NadeInHand;

	UPROPERTY(VisibleAnywhere)
		ANadeActor* Nade;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* FP_ArmMesh;

private:
	
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

	void SpawnAndAttachWeapon(UClass* SpawnClass);
	void SpawnAndAttach();

	void PrimaryEquip();
	void SecondaryEquip();
	void MeleeEquip();
	void NadeEquip();

	UClass* SpawningClass;

	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLength = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLengthTP = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float ThrowMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly)
	bool SpawnFromClass;

	UPROPERTY(EditDefaultsOnly)
	bool PickupAttachDirect;
	
	bool canFire = true;
	bool isFiring;

	
	UPROPERTY(VisibleAnywhere)
	AMeleeActor* Melee;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* CurrentWeapon = nullptr;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* PrimaryWeapon = nullptr;
	UPROPERTY(VisibleAnywhere)
	AWeaponBase* SecondaryWeapon = nullptr;

};
