// Fill out your copyright notice in the Description page of Project Settings.

#include "MannequinAI.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../Weapons/WeaponBase.h"
#include "../Weapons/BallProjectile.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine.h"


// Sets default values
AMannequinAI::AMannequinAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FP_Camera->SetupAttachment(GetCapsuleComponent());
	FP_Camera->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FP_Camera->bUsePawnControlRotation = true;
	FP_Camera->bAutoActivate = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FP_ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_ArmMesh"));
	FP_ArmMesh->SetOnlyOwnerSee(true);
	FP_ArmMesh->SetupAttachment(FP_Camera);
	FP_ArmMesh->bCastDynamicShadow = false;
	FP_ArmMesh->CastShadow = false;
	FP_ArmMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	FP_ArmMesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	Spring_Arm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Spring_Arm->SetupAttachment(GetCapsuleComponent());
	Spring_Arm->bAutoActivate = false;

	TP_Camera = CreateDefaultSubobject<UCameraComponent>("TP_Camera");
	TP_Camera->SetupAttachment(Spring_Arm);
	TP_Camera->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AMannequinAI::BeginPlay()
{
	Super::BeginPlay();

	SwitchCamPosition();

	if (SpawnFromClass)
	{
		SpawnAndAttachWeapon(GunActor);
	}
	if (InputComponent != NULL)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AMannequinAI::TriggerTimer);
		InputComponent->BindAction("Fire", IE_Released, this, &AMannequinAI::StopTriggerTimer);
		InputComponent->BindAction("Drop", IE_Pressed, this, &AMannequinAI::Drop);
		InputComponent->BindAction("ToggleCam", IE_Pressed, this, &AMannequinAI::ToggleCam);
	}
}

void AMannequinAI::SwitchCamPosition()
{
	if (IsFirstPerson)
	{
		FP_ArmMesh->SetHiddenInGame(false);
		FP_ArmMesh->SetOnlyOwnerSee(true);
		FP_Camera->SetActive(true);
		GetMesh()->SetOwnerNoSee(true);
		TP_Camera->SetActive(false);
		Spring_Arm->bUsePawnControlRotation = false;
		SpawnTraceLengthTP = 0.0f;
		if (CurrentWeapon)
			CurrentWeapon->AttachToComponent(FP_ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->GetWeaponInfo().SocketName1P);
	}
	else {
		FP_ArmMesh->SetHiddenInGame(true);
		FP_Camera->SetActive(false);
		GetMesh()->SetOwnerNoSee(false);
		TP_Camera->SetActive(true);
		Spring_Arm->bUsePawnControlRotation = true;
		SpawnTraceLengthTP = 600.f;
		if (CurrentWeapon)
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->GetWeaponInfo().SocketName3P);
	}
}
void AMannequinAI::ToggleCam()
{
	if (IsFirstPerson)
	{
		IsFirstPerson = false;
	}
	else {
		IsFirstPerson = true;
	}

	SwitchCamPosition();
}

void AMannequinAI::Drop()
{
	if (CurrentWeapon)
	{
		if (!isFiring)
		{
			CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			float ThrowSpeed;
			if ((GetVelocity().Size()*2.5f) < 200)
			{
				ThrowSpeed = 500.f;
			}
			else {
				ThrowSpeed = (GetVelocity().Size()*2.5f);
			}
			UCameraComponent* CameraToUse = IsFirstPerson ? FP_Camera : TP_Camera;
			FVector MadeVelocity = ((CameraToUse->GetForwardVector()*ThrowSpeed) + (CameraToUse->GetUpVector() * 300)) * ThrowMultiplier;
			CurrentWeapon->GetProjectileMovementComp()->Velocity = MadeVelocity;
			CurrentWeapon->GetProjectileMovementComp()->Activate();

			FTimerHandle HandleDrop;
			GetWorldTimerManager().SetTimer(HandleDrop, this, &AMannequinAI::DropSec, 0.05f, false);
			canFire = false;
		}
	}
}
void AMannequinAI::DropSec()
{
	CurrentWeapon->EnableAll();
	CurrentWeapon = nullptr;
}

void AMannequinAI::SpawnAndAttachWeapon(UClass* SpawnClass)
{
	if (SpawnClass == NULL) { UE_LOG(LogTemp, Warning, TEXT("NO CurrentWeapon GIVEN TO : %s"), *GetName()); return; }

	/*If Possible if CurrentWeapon is SET*/
	Drop();

	SpawningClass = SpawnClass;

	FTimerHandle QuickHandle;
	GetWorldTimerManager().SetTimer(QuickHandle, this, &AMannequinAI::SpawnAndAttach, 0.06f, false);
}
void AMannequinAI::SpawnAndAttach()
{
	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(SpawningClass, GetActorTransform());
	canFire = true;

	if (ensure(CurrentWeapon == nullptr)) { return; }

	CurrentWeapon->TurnOfAll();
	if (IsPlayerControlled() && IsFirstPerson)
	{
		CurrentWeapon->AttachToComponent(FP_ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->GetWeaponInfo().SocketName1P);
	}
	else
	{
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->GetWeaponInfo().SocketName3P);
	}

	CurrentWeapon->SetAnimInstances(FP_ArmMesh->GetAnimInstance(), GetMesh()->GetAnimInstance());
/*	CurrentWeapon->AnimInstance1P = FP_ArmMesh->GetAnimInstance();
	CurrentWeapon->AnimInstance3P = GetMesh()->GetAnimInstance();*/
}

// Called every frame
void AMannequinAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
// Called to bind functionality to input
void AMannequinAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMannequinAI::UnPossessed()
{
	Super::UnPossessed();

	if (ensure(CurrentWeapon == nullptr)) { return; }

	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), CurrentWeapon->GetWeaponInfo().SocketName3P);
}
void AMannequinAI::PullTrigger()
{
	if (!ensure(CurrentWeapon)) { return; }
	
	// try and fire a projectile
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = CurrentWeapon->GetSocketLocation();

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				// spawn the projectile at the muzzle
				World->SpawnActor<ABallProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	// try and play a firing animation if specified
	if (FireAnimation3P != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation3P, 1.f);
		}
	}

	/*isFiring = true;
	

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
					DrawDebugLine(GetWorld(), Start, (End + Spread), FColor::Red, true);
					/*if (HasHit)
					{
					UGameplayStatics::ApplyDamage(HitResult.GetActor(), GetWeaponInfo().Damage, PlayerCharacter->GetController(), nullptr, nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
					}
				}
			}
			else
			{
				CalculateStartAndEnd(Start, End);

				HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel3, QueryParams);
				DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);
				/*if (HasHit)
				{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), GetWeaponInfo().Damage, PlayerCharacter->GetController(), nullptr, nullptr);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
				}
			}

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
			}

			PlayFireEffects(TracerEndPoint);

			LastFireTime = GetWorld()->TimeSeconds;
		}*/
}

void AMannequinAI::TriggerTimer()
{
	if (!(CurrentWeapon) && !canFire) { return; }

	if (canHandle)
	{
		PullTrigger();
		StartTriggerTimer();
		canHandle = false;
	}
}
void AMannequinAI::StartTriggerTimer()
{
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMannequinAI::PullTrigger, 0.01, true);
}
void AMannequinAI::StopTriggerTimer()
{
	GetWorldTimerManager().ClearTimer(UnusedHandle);
	canHandle = true;
	isFiring = false;
}
void AMannequinAI::Pickup(AActor* PickedActor)
{
	SpawnAndAttachWeapon(PickedActor->GetClass());

	PickedActor->Destroy();
}

