// Fill out your copyright notice in the Description page of Project Settings.

#include "NadeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Character/Mannequin.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ANadeActor::ANadeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetSphereRadius(15.f);
	RootComponent = SphereComp;

	NadeMesh = CreateDefaultSubobject<UStaticMeshComponent>("NadeMesh");
	NadeMesh->SetupAttachment(RootComponent);

	RadialForceComp= CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->Radius = 250.f;
	RadialForceComp->ImpulseStrength= 3000.f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->SetupAttachment(RootComponent);

	ProjectileMovementComponent= CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

// Called when the game starts or when spawned
void ANadeActor::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovementComponent->Deactivate();
	SphereComp->SetSimulatePhysics(false);
}
/*void ANadeActor::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("NADE OVERLAPPPED : %s & Nade Actor Overlap: %s!!"), *OtherComp->GetName(),*OtherActor->GetName());
	if (OtherActor == GetOwner() || OtherActor == this) { return; }
	FTimerHandle OnHitHandle;
	GetWorldTimerManager().SetTimer(OnHitHandle, this, &ANadeActor::Explode, NadeInfo.HitTimerSeconds, false);
}*/
void ANadeActor::Explode()
{
	UE_LOG(LogTemp, Warning, TEXT("Explode () is called!!"));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NadeInfo.ExplosionEffect, GetActorLocation(), FRotator());
	UGameplayStatics::PlaySoundAtLocation(this, NadeInfo.ExplosionSound, GetActorLocation());
	RadialForceComp->FireImpulse();

	Destroy();
}
void ANadeActor::ThrowNade()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		PlayerCharacter = Cast<AMannequin>(MyOwner);

		UCameraComponent* CameraToUse = PlayerCharacter->GetUsedCamera();
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		FVector MadeVelocity = ((CameraToUse->GetForwardVector()*NadeInfo.ThrowSpeed) + (CameraToUse->GetUpVector() * FMath::RandRange(200.f, 350.f)));
		ProjectileMovementComponent->Velocity = MadeVelocity;
		ProjectileMovementComponent->Activate();

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &ANadeActor::ThrowSec, 0.05f, false);

		FTimerHandle LifeSpanHandle;
		GetWorldTimerManager().SetTimer(LifeSpanHandle, this, &ANadeActor::Explode, NadeInfo.LifeSpanSeconds, false);
	}
}
void ANadeActor::ThrowSec()
{
	SphereComp->SetSimulatePhysics(true);
	ProjectileMovementComponent->Deactivate();

	PlayerCharacter->SetNadeThrown(NadeInfo.NadeBasePose);

}
// Called every frame
void ANadeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

