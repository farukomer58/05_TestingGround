// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATile::PlaceActors(TSubclassOf<AActor> ActorToSpawn, int32 MinSpawn, int32 MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	FVector SpawnPoint;
	float RandomScale = FMath::RandRange(MinScale, MaxScale);
	bool found = FindEmptyLocation(SpawnPoint, Radius * RandomScale);
	if (found)
	{
		float RandomRotation = FMath::RandRange(-180.f, 180.f);
		PlaceActor(ActorToSpawn, SpawnPoint, RandomRotation, RandomScale);
	}
}
bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FVector Min = FVector(0, -2000, 0);
	FVector Max = FVector(4000, 2000, 0);
	FBox Bounds = FBox(Min, Max);
	const int MAX_ATTEMPTS = 100;

	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidatePoint, Radius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}
	return false;
}
void ATile::PlaceActor(TSubclassOf<AActor> ActorToSpawn, FVector SpawnPoint, float Rotation, float Scale)
{
	AActor* Prop = GetWorld()->SpawnActor<AActor>(ActorToSpawn);
	Prop->SetActorRelativeLocation(SpawnPoint);
	Prop->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Prop->SetActorRotation(FRotator(0, Rotation, 0));
	Prop->SetActorScale3D(FVector(Scale));
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius)
{

	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(HitResult, GlobalLocation, GlobalLocation, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel1, FCollisionShape::MakeSphere(Radius));

	/*FColor ResultColor = HasHit ? FColor::Red : FColor::Green;
	DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, ResultColor, true, 100.f);*/

	return !HasHit;
}


