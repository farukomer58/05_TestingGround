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
		GetWorldTimerManager().SetTimer(UnUsedHandle, this, &AWeaponBase::ResetCanFire, fireRate, false);

		FHitResult HitResult;
		FVector Start = PlayerCharacter->FP_Camera->GetComponentLocation();
		FVector End = (PlayerCharacter->FP_Camera->GetForwardVector() * 9999) + Start;
		bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);

		if (HasHit)
		{
			DrawDebugCone(GetWorld(), HitResult.Location, HitResult.Normal, 5, 5, 5, 25, FColor::Green, true);

			UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponDamage, PlayerCharacter->GetController(), nullptr, nullptr);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit: %s"), *HitResult.GetActor()->GetName()));
		}
		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation1P != NULL && AnimInstance1P != NULL)
		{
			AnimInstance1P->Montage_Play(FireAnimation1P, 1.f);
		}
		if (FireAnimation3P != NULL && AnimInstance3P != NULL)
		{
			AnimInstance3P->Montage_Play(FireAnimation3P, 1.f);
		}
	}
	else
	{
		return;
	}
}

//FVector AWeaponBase::GetCameraLocation()
//{
//	TInlineComponentArray<UCameraComponent*> Cameras;
//	PlayerCharacter->GetComponents<UCameraComponent>(Cameras);
//	//GetComponents<UCameraComponent>(Cameras);
//
//	for (UCameraComponent* CameraComponent : Cameras)
//	{
//		if (CameraComponent->bIsActive)
//		{
//			//CameraComponent->GetCameraView(DeltaTime, OutResult);
//			return CameraComponent->GetComponentLocation();
//		}
//	}
//}

void AWeaponBase::ResetCanFire()
{
	CanFire = true;
}


