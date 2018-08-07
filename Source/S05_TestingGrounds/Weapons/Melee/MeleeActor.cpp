// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
AMeleeActor::AMeleeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeleeMesh = CreateDefaultSubobject<UStaticMeshComponent>("NadeMesh");
	RootComponent = MeleeMesh;

	MeleeCollision = CreateDefaultSubobject<UCapsuleComponent>("MeleeCollision");
	MeleeCollision->SetupAttachment(RootComponent);

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetupAttachment(RootComponent);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMeleeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeleeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

