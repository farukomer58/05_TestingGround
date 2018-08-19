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
	bool bIsOneShot = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponClass WeaponClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CurrentAmmo = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxAmmo= 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ResetMaxAmmo = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ClipSize = 30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 50.f;;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float FireRate = 0.15f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SprayRange = 300.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsShotgun;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "IsShotgun"))
	float ShotsPerFire;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "IsShotgun"))
	float SpreadRadius;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SocketName1P = FName("WeaponSocket1P");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SocketName3P = FName("WeaponSocket3P");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName PrimaryBack = FName("PrimaryBackSocket");;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SecondaryBack= FName("SecondaryBackSocket");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName MuzzleSocket = FName("Muzzle");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName TraceBeamSocket = FName("BeamEnd");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UParticleSystem* DefaultImpactParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UParticleSystem* FleshImpactParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UParticleSystem* HeadImpactParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UParticleSystem* FireParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UParticleSystem* TracerParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation1P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation3P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UMaterialInterface* WeaponMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UTexture* ImageUI;
};

class UAnimInstance;
class UProjectileMovementComponent;
class USphereComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;

class AMannequin;

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

	void Reload();
	void ReloadMain();

	void SetPhysics(bool val);
	void SetAnimInstances(UAnimInstance* SettedAnimInstance1P, UAnimInstance* SettedAnimInstance3P);
	
	UFUNCTION(BlueprintCallable)
	void RefreshMaterial();

	UProjectileMovementComponent* GetProjectileMovementComp();

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FWeaponInfo& GetWeaponInfo();

	bool GetIsReloading();
	
	void ResetAmmo();

	FVector GetSocketLocation();

	UPROPERTY(EditAnywhere,BlueprintReadWrite/*,meta= (ClampMin = 0, ClampMax = 100)*/)
	int32 Level;
	
	
protected:
	float LastFireTime;
	FTimerHandle TimerHandle_Fire;

	UAnimInstance* AnimInstance1P;
	UAnimInstance* AnimInstance3P;

	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly)
	FAddController AddController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponInfo WeaponInfo_Zero;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponInfo WeaponInfo_One;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponInfo WeaponInfo_Two;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponInfo WeaponInfo_Three;
	FWeaponInfo WeaponInfoEmpty;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	UCapsuleComponent* InnerSphereCollision;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
	UProjectileMovementComponent* ProjectileMovementComponent;

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PlayFireEffects(FVector TracerEndPoint);

	void CalculateStartAndEnd(FVector& Start, FVector& End);

	bool CanPickup = true;
	bool IsReloading = false;
	bool IsFiring = false;

	AMannequin* PlayerCharacter = nullptr;

};
