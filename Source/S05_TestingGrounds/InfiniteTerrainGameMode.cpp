// Fill out your copyright notice in the Description page of Project Settings.

#include "InfiniteTerrainGameMode.h"
#include "AI/Navigation/NavMeshBoundsVolume.h"
#include "EngineUtils.h"
#include "ActorPool.h"


AInfiniteTerrainGameMode::AInfiniteTerrainGameMode()
{
	NavMeshBoundsVolumePool = CreateDefaultSubobject<UActorPool>("NavMeshBoundsVolumePool");
}


void AInfiniteTerrainGameMode::PopulateBoundsVolumePool()
{
	TActorIterator<ANavMeshBoundsVolume> ActorIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	while (ActorIterator)
	{
		AddToPool(*ActorIterator);
		++ActorIterator;
	}
}

void AInfiniteTerrainGameMode::AddToPool(class ANavMeshBoundsVolume* VolumeToAdd)
{
	NavMeshBoundsVolumePool->Add(VolumeToAdd);
}

void AInfiniteTerrainGameMode::IncrementScore()
{
	Score++;
}