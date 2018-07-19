// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(/*BlueprintType*/)
enum class EWeaponClass : uint8
{
	RifleFullAuto,
	SniperOneShot
};
USTRUCT(BlueprintType)
struct FAddController
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float YawMin = 0.3f;
	UPROPERTY(EditDefaultsOnly)
	float YawMax = -0.5f;
	UPROPERTY(EditDefaultsOnly)
	float PitchMin = 0.2f;
	UPROPERTY(EditDefaultsOnly)
	float PitchMax = -0.4f;
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float CurrentAmmo = 100.f;
	UPROPERTY(EditDefaultsOnly)
	float Damage = 50.f;;
	UPROPERTY(EditDefaultsOnly)
	float FireRate = 0.15f;
	UPROPERTY(EditDefaultsOnly)
	EWeaponClass WeaponClass;
	UPROPERTY(EditDefaultsOnly)
	float SprayRange = 300.f;
	UPROPERTY(EditDefaultsOnly)
	bool IsShotgun;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "IsShotgun"))
	float ShotsPerFire;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "IsShotgun"))
	float SpreadRadius;


	UPROPERTY(EditDefaultsOnly)
	FName SocketName1P;
	UPROPERTY(EditDefaultsOnly)
	FName SocketName3P;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABallProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation1P;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation3P;
};

UCLASS()
class S05_TESTINGGROUNDS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Fires a projectile. */
	UFUNCTION(BlueprintCallable)
	void OnFire();

	class UAnimInstance* AnimInstance1P;
	class UAnimInstance* AnimInstance3P;

	UPROPERTY(EditDefaultsOnly)
	FAddController AddController;

	UPROPERTY(EditDefaultsOnly)
	FWeaponInfo WeaponInfo;

private:

	void CalculateStartAndEnd(FVector& Start, FVector& End);

	void ResetCanFire();

	class UCameraComponent* GetCamera();

	bool CanFire = true;

	bool IsStarted = true;

	class AMannequin* PlayerCharacter = nullptr;

};
