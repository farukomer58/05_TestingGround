// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NadeActor.generated.h"

USTRUCT(BlueprintType)
struct FNadeInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float LifeSpanSeconds = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Damage = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ThrowSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SocketName1P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SocketName3P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName NadeBackSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UParticleSystem* ExplosionEffect;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* ExplosionSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* NadeBasePose;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* ThrowAnimation1P;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* ThrowAnimation3P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UTexture* ImageUI;
};

UCLASS()
class S05_TESTINGGROUNDS_API ANadeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANadeActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNadeInfo NadeInfo;

	class UAnimInstance* AnimInstance;

	void ThrowNade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	*/

	void Explode();

	void ThrowSec();
	
	FVector ExplodeLocation;
	bool DidHit;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* NadeMesh;
	
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComp;
	
	UPROPERTY(VisibleAnywhere)
	class URadialForceComponent* RadialForceComp;
	
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	class AMannequin* PlayerCharacter;
	
};
