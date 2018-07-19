// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "BallProjectile.h"
#include "../Character/Mannequin.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	SetRootComponent(FP_Gun);
	//FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<AMannequin>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
}
// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::OnFire()
{
	// try and fire a projectile
	/*if (projectileclass != null)
	{

		const frotator spawnrotation = fp_muzzlelocation->getcomponentrotation();
		// muzzleoffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const fvector spawnlocation = fp_muzzlelocation->getcomponentlocation();

		//set spawn collision handling override
		factorspawnparameters actorspawnparams;
		actorspawnparams.spawncollisionhandlingoverride = espawnactorcollisionhandlingmethod::adjustifpossiblebutdontspawnifcolliding;

		uworld* const world = getworld();
		if (world != null)
		{
			// spawn the projectile at the muzzle
			world->spawnactor<aballprojectile>(projectileclass, spawnlocation, spawnrotation, actorspawnparams);
		}
	}*/
	
	if (CanFire)
	{
		CanFire = false;
		FTimerHandle UnUsedHandle;
		GetWorldTimerManager().SetTimer(UnUsedHandle, this, &AWeaponBase::ResetCanFire, WeaponInfo.FireRate, false);

		bool HasHit;
		FHitResult HitResult;

		FVector Start;
		FVector End;

		if (WeaponInfo.IsShotgun)
		{
			for (int32 i = 0; i < WeaponInfo.ShotsPerFire; i++)
			{
				CalculateStartAndEnd(Start,End);
				FVector Spread = FVector(
					(FMath::RandRange(-WeaponInfo.SpreadRadius, WeaponInfo.SpreadRadius)),
					(FMath::RandRange(-WeaponInfo.SpreadRadius, WeaponInfo.SpreadRadius)),
					(FMath::RandRange(-WeaponInfo.SpreadRadius, WeaponInfo.SpreadRadius))
				);
				HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, (End + Spread), ECollisionChannel::ECC_Visibility);
				DrawDebugLine(GetWorld(), Start, (End+Spread), FColor::Red, true);

				if (HasHit)
				{
					UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponInfo.Damage, PlayerCharacter->GetController(), nullptr, nullptr);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
				}
			}
		}
		else
		{
			CalculateStartAndEnd(Start,End);

			HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);

			if (HasHit)
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponInfo.Damage, PlayerCharacter->GetController(), nullptr, nullptr);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
			}
		}
		
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
	else { return; }
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

void AWeaponBase::ResetCanFire()
{
	CanFire = true;
}


