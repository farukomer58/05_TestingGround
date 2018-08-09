// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeActor.generated.h"

USTRUCT(BlueprintType)
struct FMeleeInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Damage = 50.f;;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float FireRate = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FName SocketName1P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FName SocketName3P;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FName MeleeBackSocket;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* MeleeBasePose;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* SmashAnimation1P;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* SmashAnimation3P;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UTexture* ImageUI;
};

UCLASS()
class S05_TESTINGGROUNDS_API AMeleeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeleeActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMeleeInfo MeleeInfo;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent*	MeleeMesh;

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* MeleeCollision;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere)
	class URadialForceComponent* RadialForceComp;

};
