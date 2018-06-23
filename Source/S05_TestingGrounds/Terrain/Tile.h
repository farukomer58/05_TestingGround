// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class S05_TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable)
		void PlaceActors(TSubclassOf<AActor> ActorToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius = 300.f, float MinScale = 1, float MaxScale = 1);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	void PlaceActor(TSubclassOf<AActor> ActorToSpawn, FVector SpawnPoint, float Rotation, float Scale);

	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	bool CanSpawnAtLocation(FVector Location, float Radius);
	
};
