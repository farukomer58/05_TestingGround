// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class S05_TESTINGGROUNDS_API APickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsHealth;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PickupMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* PickupRadius;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
