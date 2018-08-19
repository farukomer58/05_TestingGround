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
#include "../S05_TestingGrounds.h"
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
	SetRootComponent(GunMesh);
	GunMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GunMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	GunMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	InnerSphereCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InnerSphereCollision"));
	InnerSphereCollision->SetupAttachment(GunMesh);
	InnerSphereCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	InnerSphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	InnerSphereCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	InnerSphereCollision->SetCanEverAffectNavigation(false);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(GunMesh);
	SphereCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(PICKUP_TRACE, ECR_Ignore);
	SphereCollision->SetCanEverAffectNavigation(false);


	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AWeaponBase::OnEndOverlap);
}
void AWeaponBase::RefreshMaterial()
{
	GunMesh->SetMaterial(0, GetWeaponInfo().WeaponMaterial);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	GunMesh->Mobility = EComponentMobility::Movable;
	GunMesh->SetSimulatePhysics(true);
	ProjectileMovementComponent->Deactivate();

	//PlayerCharacter = Cast<AMannequin>(GetOwner());
	PlayerCharacter = Cast<AMannequin>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}
void AWeaponBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == PlayerCharacter)
	{
		CanPickup = true;
		PlayerCharacter->SetCanPickup(CanPickup);
		GunMesh->SetRenderCustomDepth(true);
		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}
void AWeaponBase::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerCharacter)
	{
		CanPickup = false;
		PlayerCharacter->SetCanPickup(CanPickup);
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
void AWeaponBase::SetPhysics(bool val)
{
	GunMesh->SetSimulatePhysics(val);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AWeaponBase::OnFire()
{

	if (GetWeaponInfo().CurrentAmmo > 0)
	{
		GetWeaponInfo().CurrentAmmo = GetWeaponInfo().CurrentAmmo - 1;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s: NO AMMO LEFT"), *this->GetName());
		return;
	}

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

		if (GetWeaponInfo().IsShotgun)
		{
			for (int32 i = 0; i < GetWeaponInfo().ShotsPerFire; i++)
			{
				CalculateStartAndEnd(Start, End);
				FVector Spread = FVector(
					(FMath::RandRange(-GetWeaponInfo().SpreadRadius, GetWeaponInfo().SpreadRadius)),
					(FMath::RandRange(-GetWeaponInfo().SpreadRadius, GetWeaponInfo().SpreadRadius)),
					(FMath::RandRange(-GetWeaponInfo().SpreadRadius, GetWeaponInfo().SpreadRadius))
				);
				HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, (End + Spread), ECollisionChannel::ECC_GameTraceChannel3, QueryParams);
				//DrawDebugLine(GetWorld(), Start, (End + Spread), FColor::Red, true);
				
				if (HasHit)
				{
					AActor* HitActor = HitResult.GetActor();

					EPhysicalSurface  SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

					float ActualDamage = GetWeaponInfo().Damage;
					if (SurfaceType == SURFACE_FLESHEAD)
					{
						ActualDamage *= 4.f;
					}
					UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, PlayerCharacter->GetUsedCamera()->GetForwardVector(), HitResult, ActorOwner->GetInstigatorController(), this, DamageType);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s and the component: %s"), *HitResult.GetActor()->GetName(), *HitResult.GetComponent()->GetName()));

					UParticleSystem* SelectedEffect = nullptr;
					switch (SurfaceType)
					{
					case SURFACE_FLESHDEFAULT:
						SelectedEffect = GetWeaponInfo().FleshImpactParticle;
						break;
					case SURFACE_FLESHEAD:
						SelectedEffect = GetWeaponInfo().HeadImpactParticle;
						break;
					default:
						SelectedEffect = GetWeaponInfo().DefaultImpactParticle;
						break;
					}
					if (SelectedEffect)
					{
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.Normal.Rotation());
					}
					TracerEndPoint = HitResult.ImpactPoint;
					PlayFireEffects(TracerEndPoint);

					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
				}
			}
		}
		else
		{
			CalculateStartAndEnd(Start, End);
			HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel3, QueryParams);
			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);
			/*if (HasHit)
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), GetWeaponInfo().Damage, PlayerCharacter->GetController(), nullptr, nullptr);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
			}*/
		}
		if (HasHit && !GetWeaponInfo().IsShotgun)
		{
			AActor* HitActor = HitResult.GetActor();

			EPhysicalSurface  SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			float ActualDamage = GetWeaponInfo().Damage;
			if (SurfaceType == SURFACE_FLESHEAD)
			{
				ActualDamage *= 4.f;
			}
			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, PlayerCharacter->GetUsedCamera()->GetForwardVector(), HitResult, ActorOwner->GetInstigatorController(), this, DamageType);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s and the component: %s"), *HitResult.GetActor()->GetName(), *HitResult.GetComponent()->GetName() ));

			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
				SelectedEffect = GetWeaponInfo().FleshImpactParticle;
				break;
			case SURFACE_FLESHEAD:
				SelectedEffect = GetWeaponInfo().HeadImpactParticle;
				break;
			default:
				SelectedEffect = GetWeaponInfo().DefaultImpactParticle;
				break;
			}
			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.Normal.Rotation());
			}
			TracerEndPoint = HitResult.ImpactPoint;
		}

		if(!GetWeaponInfo().IsShotgun)
			PlayFireEffects(TracerEndPoint);

		LastFireTime = GetWorld()->TimeSeconds;
	}
}
void AWeaponBase::StartFire()
{
	if (IsReloading) { return; }

	IsFiring = true;
	float FirstDelay = FMath::Max(LastFireTime + GetWeaponInfo().FireRate - GetWorld()->TimeSeconds, 0.0f);

	if (GetWeaponInfo().bIsOneShot)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Fire, this, &AWeaponBase::OnFire, GetWeaponInfo().FireRate, false, FirstDelay);
	}
	else {
		GetWorldTimerManager().SetTimer(TimerHandle_Fire, this, &AWeaponBase::OnFire, GetWeaponInfo().FireRate, true, FirstDelay);
	}
}
void AWeaponBase::EndFire()
{
	IsFiring = false;

	if (!GetWeaponInfo().bIsOneShot)
		GetWorldTimerManager().ClearTimer(TimerHandle_Fire);

	if (!IsReloading)
	{
		if (GetWeaponInfo().CurrentAmmo <= 0 && GetWeaponInfo().MaxAmmo > 0)
			Reload();
	}
}
void AWeaponBase::PlayFireEffects(FVector TracerEndPoint)
{
	PlayerCharacter->AddControllerYawInput(FMath::RandRange(AddController.YawMin, AddController.YawMax));
	PlayerCharacter->AddControllerPitchInput(FMath::RandRange(AddController.PitchMin, AddController.PitchMax));

	if (GetWeaponInfo().TracerParticle)
	{
		FVector MuzzleLocation = GunMesh->GetSocketLocation(GetWeaponInfo().MuzzleSocket);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GetWeaponInfo().TracerParticle, MuzzleLocation);
		if (TracerComp)
			TracerComp->SetVectorParameter(GetWeaponInfo().TraceBeamSocket, TracerEndPoint);
	}

	APawn* MyOwner = Cast<APawn>(GetOwner()); //Play Cam Shake
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(GetWeaponInfo().FireCamShake);
		}
	}

	if (GetWeaponInfo().FireParticle != NULL)
	{
		UGameplayStatics::SpawnEmitterAttached(GetWeaponInfo().FireParticle, GunMesh, FName("Muzzle"));
	}
	if (GetWeaponInfo().FireSound != NULL)
	{
		UGameplayStatics::PlaySound2D(this, GetWeaponInfo().FireSound);
	}
	// try and play a firing animation if specified
	if (GetWeaponInfo().FireAnimation1P != NULL && AnimInstance1P != NULL)
	{
		AnimInstance1P->Montage_Play(GetWeaponInfo().FireAnimation1P, 1.f);
	}
	if (GetWeaponInfo().FireAnimation3P != NULL && AnimInstance3P != NULL)
	{
		AnimInstance3P->Montage_Play(GetWeaponInfo().FireAnimation3P, 1.f);
	}
}
void AWeaponBase::CalculateStartAndEnd(FVector& Start, FVector& End)
{
	Start = PlayerCharacter->GetUsedCamera()->GetComponentLocation();
	FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	float PlayerVelocityClamped = (ClampVector(PlayerVelocity, FVector(0), FVector(1))).Size();
	FVector FinalSpray = FVector(
		(FMath::RandRange(-GetWeaponInfo().SprayRange, GetWeaponInfo().SprayRange)*PlayerVelocityClamped),
		(FMath::RandRange(-GetWeaponInfo().SprayRange, GetWeaponInfo().SprayRange)*PlayerVelocityClamped),
		(FMath::RandRange(-GetWeaponInfo().SprayRange, GetWeaponInfo().SprayRange)*PlayerVelocityClamped)
	);
	End = (PlayerCharacter->GetUsedCamera()->GetForwardVector() * 9999) + Start + FinalSpray;
}
void AWeaponBase::Reload()
{
	if (IsFiring||IsReloading) { return; }

	IsReloading = true;
	AnimInstance3P->Montage_Play(GetWeaponInfo().ReloadMontage);

	FTimerHandle ReloadHandle;
	GetWorldTimerManager().SetTimer(ReloadHandle, this, &AWeaponBase::ReloadMain, 2.5f, false);
} 
void AWeaponBase::ReloadMain()
{
	float AmmoNeeded = GetWeaponInfo().ClipSize - GetWeaponInfo().CurrentAmmo;
	if (AmmoNeeded > 0)
	{
		if (GetWeaponInfo().MaxAmmo > AmmoNeeded)
		{
			GetWeaponInfo().CurrentAmmo = GetWeaponInfo().CurrentAmmo + AmmoNeeded;
			GetWeaponInfo().MaxAmmo = GetWeaponInfo().MaxAmmo - AmmoNeeded;
		}
		else {
			GetWeaponInfo().CurrentAmmo = GetWeaponInfo().CurrentAmmo + GetWeaponInfo().MaxAmmo;
			GetWeaponInfo().MaxAmmo = 0;
		}
	}
	IsReloading = false;
}

void AWeaponBase::ResetAmmo()
{
	GetWeaponInfo().CurrentAmmo = GetWeaponInfo().ClipSize;
	GetWeaponInfo().MaxAmmo = GetWeaponInfo().ResetMaxAmmo;
}


void AWeaponBase::SetAnimInstances(UAnimInstance* SettedAnimInstance1P, UAnimInstance* SettedAnimInstance3P)
{
	AnimInstance1P = SettedAnimInstance1P;
	AnimInstance3P = SettedAnimInstance3P;
}

UProjectileMovementComponent* AWeaponBase::GetProjectileMovementComp()
{
	return ProjectileMovementComponent;
}
bool AWeaponBase::GetIsReloading()
{
	return IsReloading;
}
FWeaponInfo& AWeaponBase::GetWeaponInfo()
{
	if (Level == 0)
	{
		return WeaponInfo_Zero;
	}
	else if (Level == 1)
	{
		return WeaponInfo_One;
	}
	else if (Level == 2)
	{
		return WeaponInfo_Two;
	}
	else if (Level == 3)
	{
		return WeaponInfo_Three;
	}

	return WeaponInfoEmpty;
}
FVector AWeaponBase::GetSocketLocation()
{
	return GunMesh->GetSocketLocation(GetWeaponInfo().MuzzleSocket);
}