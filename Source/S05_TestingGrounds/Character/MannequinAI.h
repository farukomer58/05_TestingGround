// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MannequinAI.generated.h"


UCLASS()
class S05_TESTINGGROUNDS_API AMannequinAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMannequinAI();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurrentHealth = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxHealth = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsAiming = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsDead;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TSubclassOf<class ABallProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation3P;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* FP_ArmMesh;

private:

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

	AWeaponBase* CurrentWeapon;

};
