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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWeaponBase> GunActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* FP_ArmMesh;

private:
	bool canHandle = true;

	FTimerHandle UnusedHandle;

	AWeaponBase* Weapon;
};
