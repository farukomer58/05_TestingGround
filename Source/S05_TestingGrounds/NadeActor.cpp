// Fill out your copyright notice in the Description page of Project Settings.

#include "NadeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Character/Mannequin.h"


// Sets default values
ANadeActor::ANadeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NadeMesh = CreateDefaultSubobject<UStaticMeshComponent>("NadeMesh");
	RootComponent = NadeMesh;

	SphereComp= CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetupAttachment(RootComponent);

	RadialForceComp= CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->SetupAttachment(RootComponent);

	ProjectileMovementComponent= CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

// Called when the game starts or when spawned
void ANadeActor::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovementComponent->Deactivate();
	NadeMesh->SetSimulatePhysics(false);
}
void ANadeActor::ThrowNade()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		PlayerCharacter = Cast<AMannequin>(MyOwner);

		UCameraComponent* CameraToUse = PlayerCharacter->IsFirstPerson ? PlayerCharacter->FP_Camera : PlayerCharacter->TP_Camera;
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		FVector MadeVelocity = ((CameraToUse->GetForwardVector()*NadeInfo.ThrowSpeed) + (CameraToUse->GetUpVector() * FMath::RandRange(200.f, 350.f)));
		ProjectileMovementComponent->Velocity = MadeVelocity;
		ProjectileMovementComponent->Activate();

		
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &ANadeActor::ThrowSec, 0.05f, false, 0.f);
	}
}
void ANadeActor::ThrowSec()
{
	NadeMesh->SetSimulatePhysics(true);
	PlayerCharacter->NadeInHand = false;
	PlayerCharacter->Nade = nullptr;
}


// Called every frame
void ANadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

