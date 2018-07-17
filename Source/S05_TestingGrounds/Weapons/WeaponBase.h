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


USTRUCT(/*BlueprintType*/)
struct FWeaponInfo
{
	GENERATED_USTRUCT_BODY()

public:
	
	float CurrentAmmo = 100.f;
	float Damage = 50.f;;
	float FireRate = 0.15f;
	EWeaponClass WeaponClass;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float WeaponDamage = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float fireRate = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	EWeaponClass WeaponClass = EWeaponClass::RifleFullAuto;

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

	class UAnimInstance* AnimInstance1P;
	class UAnimInstance* AnimInstance3P;

private:

	//FVector GetCameraLocation();

	void ResetCanFire();

	bool CanFire = true;
	bool IsStarted = true;
	//

	class AMannequin* PlayerCharacter = nullptr;

};
