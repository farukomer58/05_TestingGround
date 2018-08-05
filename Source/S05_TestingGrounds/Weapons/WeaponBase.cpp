// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"

#include "BallProjectile.h"
#include "../Character/Mannequin.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Engine.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	// GunMesh->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	SetRootComponent(GunMesh);
	GunMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GunMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	GunMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	//GunMesh->SetupAttachment(RootComponent);

	InnerSphereCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InnerSphereCollision"));
	InnerSphereCollision->SetupAttachment(GunMesh);
	InnerSphereCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InnerSphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	InnerSphereCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(GunMesh);
	SphereCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnEndOverlap);
}
// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	GunMesh->Mobility = EComponentMobility::Movable;
	GunMesh->SetSimulatePhysics(true);
	ProjectileMovementComponent->Deactivate();

	PlayerCharacter = Cast<AMannequin>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
}
void AWeaponBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerCharacter)
	{
		CanPickup = true;
		PlayerCharacter->CanPickup = CanPickup;
		GunMesh->SetRenderCustomDepth(true);
		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}
void AWeaponBase::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerCharacter)
	{
		CanPickup = false;
		PlayerCharacter->CanPickup = CanPickup;
		GunMesh->SetRenderCustomDepth(false);
		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}
void AWeaponBase::TurnOfAll()
{
	GunMesh->SetSimulatePhysics(false);
	GunMesh->SetRenderCustomDepth(false);
	CanPickup = false;
	SphereCollision->bGenerateOverlapEvents = false;
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponBase::EnableAll()
{
	GunMesh->SetSimulatePhysics(true);
	SphereCollision->bGenerateOverlapEvents = true;
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AWeaponBase::OnFire()
{
	AActor* ActorOwner = GetOwner();

	if (ActorOwner)
	{
		bool HasHit;
		FVector Start;
		FVector End;

		FVector TracerEndPoint = End;

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(ActorOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		if (WeaponInfo.IsShotgun)
		{
			for (int32 i = 0; i < WeaponInfo.ShotsPerFire; i++)
			{
				CalculateStartAndEnd(Start, End);
				FVector Spread = FVector(
					(FMath::RandRange(-WeaponInfo.SpreadRadius, WeaponInfo.SpreadRadius)),
					(FMath::RandRange(-WeaponInfo.SpreadRadius, WeaponInfo.SpreadRadius)),
					(FMath::RandRange(-WeaponInfo.SpreadRadius, WeaponInfo.SpreadRadius))
				);
				HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, (End + Spread), ECollisionChannel::ECC_GameTraceChannel3, QueryParams);
				DrawDebugLine(GetWorld(), Start, (End + Spread), FColor::Red, true);
				if (HasHit)
				{
					UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponInfo.Damage, PlayerCharacter->GetController(), nullptr, nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
				}
			}
		}
		else
		{
			CalculateStartAndEnd(Start, End);

			HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel3, QueryParams);
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);

			if (HasHit)
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponInfo.Damage, PlayerCharacter->GetController(), nullptr, nullptr);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
			}
		}
		//these are special Headshot Particles

		/*if (HasHit)
		{
			AActor* HitActor = HitResult.GetActor();

			EPhysicalSurface  SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			float ActualDamage = WeaponInfo.Damage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, HitResult, ActorOwner->GetInstigatorController(), this, DamageType);
			UParticleSystem* SelectedEffect = nullptr;

			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactParticle;
				break;
			default:
				SelectedEffect = DefaultImpactParticle;
				break;
			}

			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.Normal.Rotation());
			}

			TracerEndPoint = HitResult.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.f, 0, 2.f);

		PlayFireEffects(TracerEndPoint);

		LastFireTime = GetWorld()->TimeSeconds;
	}*/

		PlayFireEffects(TracerEndPoint);

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

void AWeaponBase::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + WeaponInfo.FireRate - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_Fire, this, &AWeaponBase::OnFire, WeaponInfo.FireRate, true, FirstDelay);
}
void AWeaponBase::EndFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
}
void AWeaponBase::PlayFireEffects(FVector TracerEndPoint)
{
	PlayerCharacter->AddControllerYawInput(FMath::RandRange(AddController.YawMin, AddController.YawMax));
	PlayerCharacter->AddControllerPitchInput(FMath::RandRange(AddController.PitchMin, AddController.PitchMax));

	// try and play the sound if specified
	if (WeaponInfo.FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponInfo.FireSound, GetActorLocation());
	}
	// try and play a firing animation if specified
	if (WeaponInfo.FireAnimation1P != NULL && AnimInstance1P != NULL)
	{
		AnimInstance1P->Montage_Play(WeaponInfo.FireAnimation1P, 1.f);
	}
	if (WeaponInfo.FireAnimation3P != NULL && AnimInstance3P != NULL)
	{
		AnimInstance3P->Montage_Play(WeaponInfo.FireAnimation3P, 1.f);
	}
}

void AWeaponBase::CalculateStartAndEnd(FVector& Start, FVector& End)
{
	Start = GetCamera()->GetComponentLocation();
	FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	float PlayerVelocityClamped = (ClampVector(PlayerVelocity, FVector(0), FVector(1))).Size();
	FVector FinalSpray = FVector(
		(FMath::RandRange(-WeaponInfo.SprayRange, WeaponInfo.SprayRange)*PlayerVelocityClamped),
		(FMath::RandRange(-WeaponInfo.SprayRange, WeaponInfo.SprayRange)*PlayerVelocityClamped),
		(FMath::RandRange(-WeaponInfo.SprayRange, WeaponInfo.SprayRange)*PlayerVelocityClamped)
	);
	End = (GetCamera()->GetForwardVector() * 9999) + Start + FinalSpray;
}
UCameraComponent* AWeaponBase::GetCamera()
{
	if (PlayerCharacter->IsFirstPerson)
	{
		return PlayerCharacter->FP_Camera;
	}
	else {
		return PlayerCharacter->TP_Camera;
	}
}


