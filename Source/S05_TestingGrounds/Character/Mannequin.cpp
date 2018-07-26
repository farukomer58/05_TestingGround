// Fill out your copyright notice in the Description page of Project Settings.

#include "Mannequin.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "../Weapons/WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine.h"


// Sets default values
AMannequin::AMannequin()
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

	CameraCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CameraCollisionComp"));
	CameraCollisionComp->SetupAttachment(Spring_Arm);

}

// Called when the game starts or when spawned
void AMannequin::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnFromClass)
	{
		SpawnAndAttachWeapon(GunActor);
	}
	if (InputComponent != NULL)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AMannequin::TriggerTimer);
		InputComponent->BindAction("Fire", IE_Released, this, &AMannequin::StopTriggerTimer);
		InputComponent->BindAction("TryPickup", IE_Pressed, this, &AMannequin::TryPickup);
	}
}

void AMannequin::TryPickup()
{
	if (CanPickup)
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddIgnoredActor(this);

		UCameraComponent* UsedCamera = IsFirstPerson ? FP_Camera : TP_Camera;
		FVector Start = UsedCamera->GetComponentLocation();
		if (SpawnTraceLengthTP > 0.0f)
		{
			SpawnTraceLength = SpawnTraceLengthTP;
		}
		FVector End = (UsedCamera->GetForwardVector() * SpawnTraceLength) + Start;
		bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECollisionChannel::ECC_Visibility, CollisionObjectQueryParams);
		
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 4.f);

		if (HasHit)
		{
			if (Cast<AWeaponBase>(HitResult.GetActor()))
			{
				UE_LOG(LogTemp, Error, TEXT("You can pickup: %s"), *HitResult.GetActor()->GetName());
				Pickup(HitResult.GetActor());
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("You can NOT pickup: %s"), *HitResult.GetActor()->GetName());
			}
		}

	}
}

void AMannequin::SpawnAndAttachWeapon(UClass* SpawnClass)
{
	if (SpawnClass == NULL) { UE_LOG(LogTemp, Warning, TEXT("NO CurrentWeapon GIVEN TO : %s"), *GetName()); return; }
	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(SpawnClass,GetActorTransform());
	if (ensure(CurrentWeapon == nullptr)) { return; }
	CurrentWeapon->TurnOfAll();

	if (IsPlayerControlled() && IsFirstPerson)
	{
		UE_LOG(LogTemp, Error, TEXT("SPawn begin Attaching to : %s"), *CurrentWeapon->WeaponInfo.SocketName1P.ToString());
		CurrentWeapon->AttachToComponent(FP_ArmMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->WeaponInfo.SocketName1P);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" %s : SPawn begin Attaching to : %s"), *this->GetName(), *CurrentWeapon->WeaponInfo.SocketName3P.ToString());
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CurrentWeapon->WeaponInfo.SocketName3P);
		//CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("WeaponSocket1P"));
	}

	CurrentWeapon->AnimInstance1P = FP_ArmMesh->GetAnimInstance();
	CurrentWeapon->AnimInstance3P = GetMesh()->GetAnimInstance();
}

// Called every frame
void AMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ensure(CameraCollisionComp)) { return; }

	UE_LOG(LogTemp, Warning, TEXT("is Ticking..."));

	FOverlapInfo OverlapInfo;
	CameraCollisionComp->IsOverlappingComponent(OverlapInfo);

	UE_LOG(LogTemp, Warning, TEXT("camera hit : %s"), *OverlapInfo.OverlapInfo.Component->GetName());

	if (!IsFirstPerson)
		if (OverlapInfo.OverlapInfo.Component == GetCapsuleComponent())
		{
			GetMesh()->bOwnerNoSee = true;
		}
		else {
			GetMesh()->bOwnerNoSee = false;
		}

}
// Called to bind functionality to input
void AMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
void AMannequin::UnPossessed()
{
	Super::UnPossessed();

	if (ensure(CurrentWeapon == nullptr)) { return; }

	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), CurrentWeapon->WeaponInfo.SocketName3P);
}
void AMannequin::PullTrigger()
{
	if (!ensure(CurrentWeapon)) { return; }
	CurrentWeapon->OnFire(this);
}
void AMannequin::TriggerTimer()
{
	if (!(CurrentWeapon)) { return; }

	if (canHandle)
	{
		PullTrigger();
		StartTriggerTimer();
		canHandle = false;
	}
}
void AMannequin::StartTriggerTimer()
{
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AMannequin::PullTrigger, 0.01, true);
}
void AMannequin::StopTriggerTimer()
{
	GetWorldTimerManager().ClearTimer(UnusedHandle);
	canHandle = true;
}

void AMannequin::Pickup(AActor* PickedActor)
{
	SpawnAndAttachWeapon(PickedActor->GetClass());

	PickedActor->Destroy();
}

