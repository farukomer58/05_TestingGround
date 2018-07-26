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


	UPROPERTY(EditDefaultsOnly)
	bool IsFirstPerson;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FP_Camera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* Spring_Arm;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TP_Camera;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "SpawnFromClass"))
	TSubclassOf<class AWeaponBase> GunActor;

	void Pickup(AActor* PickedActor);

	bool CanPickup = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CameraCollisionComp;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* FP_ArmMesh;

private:

	void TryPickup();

	void SpawnAndAttachWeapon(UClass* SpawnClass);

	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLength = 300.f;
	UPROPERTY(EditDefaultsOnly)
	float SpawnTraceLengthTP = 300.f;

	UPROPERTY(EditDefaultsOnly)
	bool SpawnFromClass;

	bool canHandle = true;

	FTimerHandle UnusedHandle;

	AWeaponBase* CurrentWeapon;
};
