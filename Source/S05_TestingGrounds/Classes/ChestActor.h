// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChestActor.generated.h"


UCLASS()
class S05_TESTINGGROUNDS_API AChestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChestActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OpenChest();

	UPROPERTY(BlueprintReadWrite)
	bool CanOpen = true;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float CostChest = 500.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void Open();

	/*UPROPERTY(EditAnywhere)
	TArray<class AWeaponBase*> PossibleWeapons;*/


};
