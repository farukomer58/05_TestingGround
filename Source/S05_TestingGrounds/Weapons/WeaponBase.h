// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(/*BlueprintType*/)
enum class EWeaponClass : uint8
{
	PrimaryWeapon,
	SecondaryWeapon,
	Melee,
	Grenade
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurrentAmmo = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 50.f;;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FireRate = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponClass WeaponClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SprayRange = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsShotgun;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "IsShotgun"))
	float ShotsPerFire;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "IsShotgun"))
	float SpreadRadius;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SocketName1P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SocketName3P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName PrimaryBack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SecondaryBack;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UTexture* ImageUI;
};

UCLASS()
class S05_TESTINGGROUNDS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
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

	void StartFire();
	void EndFire();
	
	void TurnOfAll();
	void EnableAll();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	class UAnimInstance* AnimInstance1P;
	class UAnimInstance* AnimInstance3P;

	UPROPERTY(EditDefaultsOnly)
	FAddController AddController;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FWeaponInfo WeaponInfo;
protected:
	void PlayFireEffects(FVector TracerEndPoint);

	float LastFireTime;

	FTimerHandle TimerHandle_Fire;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class UCapsuleComponent* InnerSphereCollision;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	class USphereComponent* SphereCollision;

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CalculateStartAndEnd(FVector& Start, FVector& End);

	class UCameraComponent* GetCamera();

	bool CanPickup = true;

	class AMannequin* PlayerCharacter = nullptr;

};
