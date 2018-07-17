// Fill out your copyright notice in the Description page of Project Settings.

#include "Mannequin.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Engine.h"
#include "../Weapons/WeaponBase.h"


// Sets default values
AMannequin::AMannequin()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FP_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FP_Camera->SetupAttachment(GetCapsuleComponent());
	FP_Camera->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FP_Camera->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FP_ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_ArmMesh"));
	FP_ArmMesh->SetOnlyOwnerSee(true);
	FP_ArmMesh->SetupAttachment(FP_Camera);
	FP_ArmMesh->bCastDynamicShadow = false;
	FP_ArmMesh->CastShadow = false;
	FP_ArmMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	FP_ArmMesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

}

// Called when the game starts or when spawned
void AMannequin::BeginPlay()
{
	Super::BeginPlay();

	if (GunActor == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO WEAPON GIVEN TO : %s"), *GetName());
		return;
	}
	Weapon = GetWorld()->SpawnActor<AWeaponBase>(GunActor);
	if (ensure(Weapon == nullptr)) { return; }
	if (IsPlayerControlled() && IsFirstPerson)
	{
		Weapon->AttachToComponent(FP_ArmMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), Weapon->SocketName1P);
	}
	else
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), Weapon->SocketName3P);
	}

	Weapon->AnimInstance1P = FP_ArmMesh->GetAnimInstance();
	Weapon->AnimInstance3P = GetMesh()->GetAnimInstance();

	if (InputComponent != NULL)
		InputComponent->BindAction("Fire", IE_Pressed, this, &AMannequin::TriggerTimer);
	if (InputComponent != NULL)
		InputComponent->BindAction("Fire", IE_Released, this, &AMannequin::StopTriggerTimer);
}

// Called every frame
void AMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Called to bind functionality to input
void AMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMannequin::UnPossessed()
{
	Super::UnPossessed();

	if (ensure(Weapon == nullptr)) { return; }
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("GripPoint_0"));
}
void AMannequin::PullTrigger()
{
	Weapon->OnFire();
	
}
void AMannequin::TriggerTimer()
{
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
