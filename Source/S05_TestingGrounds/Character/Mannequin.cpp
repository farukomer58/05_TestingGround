// Fill out your copyright notice in the Description page of Project Settings.

#include "Mannequin.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GamePlaystatics.h"
#include "../Weapons/WeaponBase.h"
#include "../Weapons/Melee/MeleeActor.h"
#include "../NadeActor.h"
#include "../Public/PickupActor.h"
#include "../Classes/ChestActor.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine.h"
#include "../S05_TestingGrounds.h"


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

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.f;
}

// Called when the game starts or when spawned
void AMannequin::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = GetUsedCamera()->FieldOfView;

	SwitchCamPosition();

	if (SpawnFromClass)
	{
		SpawnAndAttachWeapon(GunActor);
	}
	if (NadeActor)
	{
		Nade = GetWorld()->SpawnActor<ANadeActor>(NadeActor);
		Nade->SetOwner(this);
		AttachNadeToBack(Nade);
	}
	if (MeleeActor)
	{
		Melee = GetWorld()->SpawnActor<AMeleeActor>(MeleeActor);
		Melee->SetOwner(this);
		AttachNadeToBack(Melee);
	}

	if (InputComponent != NULL)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AMannequin::StartFire);
		InputComponent->BindAction("Fire", IE_Released, this, &AMannequin::EndFire);
		InputComponent->BindAction("TryPickup", IE_Pressed, this, &AMannequin::TryPickup);
		InputComponent->BindAction("Drop", IE_Pressed, this, &AMannequin::Drop);
		InputComponent->BindAction("ToggleCam", IE_Pressed, this, &AMannequin::ToggleCam);
		InputComponent->BindAction("Reload", IE_Pressed, this, &AMannequin::Reload);

		InputComponent->BindAction("PrimaryEquip", IE_Pressed, this, &AMannequin::PrimaryEquip);
		InputComponent->BindAction("SecondaryEquip", IE_Pressed, this, &AMannequin::SecondaryEquip);
		InputComponent->BindAction("MeleeEquip", IE_Pressed, this, &AMannequin::MeleeEquip);
		InputComponent->BindAction("NadeEquip", IE_Pressed, this, &AMannequin::NadeEquip);
	}
}

void AMannequin::SwitchCamPosition()
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
		
		AttachToHand(CurrentWeapon);
		if(NadeInHand)
			AttachNadeToHand(Nade);
		if (MeleeInHand)
		AttachNadeToHand(Melee);

	}
	else {
		FP_ArmMesh->SetHiddenInGame(true);
		FP_Camera->SetActive(false);
		GetMesh()->SetOwnerNoSee(false);
		TP_Camera->SetActive(true);
		Spring_Arm->bUsePawnControlRotation = true;
		SpawnTraceLengthTP = 600.f;
		bUseControllerRotationYaw = false;

		AttachToHand(CurrentWeapon);
		if (NadeInHand)
			AttachNadeToHand(Nade);
		if (MeleeInHand)
			AttachNadeToHand(Melee);
	}
}
void AMannequin::ToggleCam()
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
void AMannequin::Reload()
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponInfo().CurrentAmmo < CurrentWeapon->GetWeaponInfo().ClipSize && CurrentWeapon->GetWeaponInfo().MaxAmmo>0)
		{
			CurrentWeapon->Reload();
		}
	}
}


void AMannequin::TryPickup()
{
		FHitResult HitResult;
		FCollisionQueryParams CollisionObjectQueryParams;
		CollisionObjectQueryParams.AddIgnoredActor(this);
		if(CurrentWeapon)
			CollisionObjectQueryParams.AddIgnoredActor(CurrentWeapon);

		FVector Start = GetUsedCamera()->GetComponentLocation();
		if (SpawnTraceLengthTP > 0.0f)
			SpawnTraceLength = SpawnTraceLengthTP;
		FVector End = (GetUsedCamera()->GetForwardVector() * SpawnTraceLength) + Start;
		
		bool HasHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, PICKUP_TRACE, CollisionObjectQueryParams);
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.5f);

		if (HasHit)
		{
			AChestActor* Chest = Cast<AChestActor>(HitResult.GetActor());
			APickupActor* PickupActor = Cast<APickupActor>(HitResult.GetActor());
			if (Cast<AWeaponBase>(HitResult.GetActor()))
			{
				//UE_LOG(LogTemp, Error, TEXT("You can pickup: %s"), *HitResult.GetActor()->GetName());
				Pickup(HitResult.GetActor());
			}
			if (Cast<ANadeActor>(HitResult.GetActor()))
			{
				AmountOfNade = 3.f;
				if (Nade == nullptr)
				{
					Nade = GetWorld()->SpawnActor<ANadeActor>(NadeActor);
					Nade->SetOwner(this);
					NadeEquip();
				}
				HitResult.GetActor()->Destroy();
			}
			else if (PickupActor)
			{
				if (PickupActor->IsHealth)
				{
					if (CurrentHealth < MaxHealth)
					{
						CurrentHealth = MaxHealth;
						PickupActor->Destroy();
						UGameplayStatics::PlaySound2D(this, HealthPickupSound);
					}
					else {
						if (ErrorHandle.IsValid()) { return; }
						ShouldError = true;
						ErrorMessage = "ALREADY FULL HEALTH!!";
						GetWorldTimerManager().SetTimer(ErrorHandle, this, &AMannequin::ResetError, 2.0f, false);
					}
				}
				else {
					if (CurrentWeapon)
					{
						if (CurrentWeapon->GetWeaponInfo().CurrentAmmo < CurrentWeapon->GetWeaponInfo().ClipSize || CurrentWeapon->GetWeaponInfo().MaxAmmo < CurrentWeapon->GetWeaponInfo().ResetMaxAmmo)
						{
							CurrentWeapon->ResetAmmo();
							PickupActor->Destroy();
							UGameplayStatics::PlaySound2D(this, AmmoPickupSound);
						}
						else {
							if (ErrorHandle.IsValid()) { return; }
							ShouldError = true;
							ErrorMessage = "AlreadyFullAmmo!!";
							GetWorldTimerManager().SetTimer(ErrorHandle, this, &AMannequin::ResetError, 2.0f, false);
						}
					}
				}
			}
			else if (Chest)
			{
				if (Chest->CanOpen && CurrentMoney >= Chest->CostChest)
				{
					Chest->OpenChest();
				}
				else {
					if (ErrorHandle.IsValid()) { return; }
					ShouldError = true;
					ErrorMessage = "CAN NOT OPEN CHEST!!";
					GetWorldTimerManager().SetTimer(ErrorHandle, this, &AMannequin::ResetError, 2.0f, false);
				}
			}
		}
}
void AMannequin::Pickup(AActor* PickedActor)
{
	SpawnAndAttachWeapon(PickedActor->GetClass());

	PickedActor->Destroy();
}

void AMannequin::SpawnAndAttachWeapon(UClass* SpawnClass)
{
	if (SpawnClass == NULL) { UE_LOG(LogTemp, Warning, TEXT("NO CurrentWeapon GIVEN TO : %s"), *GetName()); return; }
	
	/*SpawningClass = SpawnClass;
	if (CurrentWeapon)
	{
		if (SpawnClass == CurrentWeapon->GetClass()|| (PrimaryWeapon && SecondaryWeapon))
		{
			Drop();
		}
	}*/
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(SpawnClass, GetActorTransform(), SpawnParams);
	bool ShouldSpawn = true;

	if (CurrentWeapon && SpawnedWeapon)
	{
		if (SpawnedWeapon->GetClass() == CurrentWeapon->GetClass() || SpawnedWeapon->GetWeaponInfo().WeaponClass == CurrentWeapon->GetWeaponInfo().WeaponClass || (PrimaryWeapon && SecondaryWeapon))
		{
			if (SpawnedWeapon->GetWeaponInfo().WeaponClass == EWeaponClass::PrimaryWeapon && SecondaryInHand)
			{
				if (ErrorHandle.IsValid()) { return; }
				ShouldSpawn= false;
				ShouldError = true;
				ErrorMessage = "First Drop u PrimaryWeapon to pick this Up!!!";
				GetWorldTimerManager().SetTimer(ErrorHandle, this, &AMannequin::ResetError, 2.0f, false);
			}
			else if(SpawnedWeapon->GetWeaponInfo().WeaponClass == EWeaponClass::SecondaryWeapon && PrimaryInHand) {
				if (ErrorHandle.IsValid()) { return; }
				ShouldSpawn = false;
				ShouldError = true;
				ErrorMessage = "First Drop u SecondaryWeapon to pick this Up!!!";
				GetWorldTimerManager().SetTimer(ErrorHandle, this, &AMannequin::ResetError, 2.0f, false);
			}
			else {
				ShouldSpawn = true;
				ShouldError = false;
				Drop();
			}
		}
	}
	if (!ShouldSpawn) { UE_LOG(LogTemp, Warning, TEXT("I WILL NOT SPAWNN THE WANTED WEAPONN!!!"));  return; }

	FTimerHandle QuickHandle;
	GetWorldTimerManager().SetTimer(QuickHandle, this, &AMannequin::SpawnAndAttach, 0.06f, false);
}
void AMannequin::SpawnAndAttach()
{
	if (ensure(SpawnedWeapon == nullptr)) { return; }

	SpawnedWeapon->SetOwner(this);
	canFire = true;
	
	if (SpawnedWeapon->GetWeaponInfo().WeaponClass == EWeaponClass::PrimaryWeapon)
	{
		PrimaryWeapon = SpawnedWeapon;
		SetAndAttach(PrimaryWeapon);
	}
	if (SpawnedWeapon->GetWeaponInfo().WeaponClass == EWeaponClass::SecondaryWeapon)
	{
		SecondaryWeapon = SpawnedWeapon;
		SetAndAttach(SecondaryWeapon);
	}
}
void AMannequin::AttachToHand(AWeaponBase* Weapon)
{
	if (Weapon == nullptr) { return; }

	CurrentWeapon = Weapon;

	if (IsPlayerControlled() && IsFirstPerson)
	{
		Weapon->AttachToComponent(FP_ArmMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetWeaponInfo().SocketName1P);
	}
	else
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetWeaponInfo().SocketName3P);
	}

	bUseControllerRotationYaw = true;
	WeaponCombat = true;

	if(Weapon->GetWeaponInfo().WeaponClass==EWeaponClass::PrimaryWeapon)
		PrimaryInHand = true;
	if (Weapon->GetWeaponInfo().WeaponClass == EWeaponClass::SecondaryWeapon)
		SecondaryInHand = true;
}
void AMannequin::AttachToBack(AWeaponBase* Weapon)
{
	if (Weapon == nullptr) { return; }

	if (Weapon->GetWeaponInfo().WeaponClass== EWeaponClass::PrimaryWeapon)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetWeaponInfo().PrimaryBack);
	}
	else
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->GetWeaponInfo().SecondaryBack);
	}

	
	if (Weapon->GetWeaponInfo().WeaponClass == EWeaponClass::PrimaryWeapon)
		PrimaryInHand = false;
	if (Weapon->GetWeaponInfo().WeaponClass == EWeaponClass::SecondaryWeapon)
		SecondaryInHand = false;

	if(PrimaryInHand|| SecondaryInHand)
	{ 
		WeaponCombat = true;
		bUseControllerRotationYaw = true;
	}
	else {
		WeaponCombat = false;
		bUseControllerRotationYaw = false;
	}
}
void AMannequin::SetAndAttach(AWeaponBase* Weapon)
{
	Weapon->TurnOfAll();
	Weapon->SetAnimInstances(FP_ArmMesh->GetAnimInstance(), GetMesh()->GetAnimInstance());

	if (Weapon->GetWeaponInfo().WeaponClass == EWeaponClass::PrimaryWeapon)
	{
		/*if (SecondaryInHand)
		{
			if (PickupAttachDirect)
			{
				AttachToBack(SecondaryWeapon);
				AttachToHand(Weapon);
			}
			else {
				AttachToBack(Weapon);
			}
		}
		else if (NadeInHand)
		{
			AttachNadeToBack(Nade);
			AttachToHand(Weapon);
		}
		else if (MeleeInHand)
		{
			AttachNadeToBack(Melee);
			AttachToHand(Weapon);
		}
		else {
			AttachToHand(Weapon);
		}*/PrimaryEquip();
	
	}
	else {
		/*if (PrimaryInHand)
		{
			if (PickupAttachDirect)
			{
				AttachToBack(PrimaryWeapon);
				AttachToHand(Weapon);
			}
			else {
				AttachToBack(Weapon);
			}
		}
		else if (NadeInHand)
		{
			AttachNadeToBack(Nade);
			AttachToHand(Weapon);
		}
		else if (MeleeInHand)
		{
			AttachNadeToBack(Melee);
			AttachToHand(Weapon);
		}
		else {
			AttachToHand(Weapon);
		}
	*/SecondaryEquip();
	}
}

void AMannequin::AttachNadeToHand(AActor* ActorToAttach)
{
	if (ActorToAttach == nullptr) { return; }

	ANadeActor* CastNade = Cast<ANadeActor>(ActorToAttach);
	if (CastNade)
	{
		if (IsPlayerControlled() && IsFirstPerson)
		{
			ActorToAttach->AttachToComponent(FP_ArmMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, CastNade->NadeInfo.SocketName1P);
		}
		else
		{
			ActorToAttach->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CastNade->NadeInfo.SocketName3P);
		}

		bUseControllerRotationYaw = true;
		NadeInHand = true;
		WeaponCombat = false;
		CurrentWeapon = nullptr;
		if (Melee)
		{
			GetMesh()->GetAnimInstance()->Montage_IsPlaying(Melee->MeleeInfo.MeleeBasePose);
			GetMesh()->GetAnimInstance()->Montage_Stop(0.05f,Melee->MeleeInfo.MeleeBasePose);
		}
		GetMesh()->GetAnimInstance()->Montage_Play(Nade->NadeInfo.NadeBasePose);
	}
	else {
		AMeleeActor* CastMelee = Cast<AMeleeActor>(ActorToAttach);
		if (CastMelee)
		{
			if (IsPlayerControlled() && IsFirstPerson)
			{
				ActorToAttach->AttachToComponent(FP_ArmMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, CastMelee->MeleeInfo.SocketName1P);
			}
			else
			{
				ActorToAttach->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CastMelee->MeleeInfo.SocketName3P);
			}

			bUseControllerRotationYaw = true;
			MeleeInHand = true;
			WeaponCombat = false;
			CurrentWeapon = nullptr;

			if (Nade)
			{
				GetMesh()->GetAnimInstance()->Montage_IsPlaying(Nade->NadeInfo.NadeBasePose);
				GetMesh()->GetAnimInstance()->Montage_Stop(0.05f, Nade->NadeInfo.NadeBasePose);
			}

			GetMesh()->GetAnimInstance()->Montage_Play(Melee->MeleeInfo.MeleeBasePose);

		}
	}

}
void AMannequin::AttachNadeToBack(AActor* ActorToAttach)
{
	if (ActorToAttach == nullptr) { return; }

	ANadeActor* CastNade = Cast<ANadeActor>(ActorToAttach);
	if (CastNade)
	{
		ActorToAttach->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CastNade->NadeInfo.NadeBackSocket);
	
		bUseControllerRotationYaw = false;
		NadeInHand = false;

		GetMesh()->GetAnimInstance()->Montage_Stop(0.05f, CastNade->NadeInfo.NadeBasePose);
	}
	else
	{
		AMeleeActor* CastMelee = Cast<AMeleeActor>(ActorToAttach);
		if (CastMelee)
		{
			ActorToAttach->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CastMelee->MeleeInfo.MeleeBackSocket);
			
			bUseControllerRotationYaw = false;
			MeleeInHand = false;

			GetMesh()->GetAnimInstance()->Montage_Stop(0.05f, CastMelee->MeleeInfo.MeleeBasePose);

		}
	}
}

void AMannequin::Drop()
{
	if (CurrentWeapon == nullptr) { return; }
	if (CurrentWeapon->GetIsReloading()) { return; }

	if (PrimaryInHand)
	{
		if (!isFiring)
		{
			PrimaryWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

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
			PrimaryWeapon->GetProjectileMovementComp()->Velocity = MadeVelocity;
			PrimaryWeapon->GetProjectileMovementComp()->Activate();

			FTimerHandle HandleDrop;
			GetWorldTimerManager().SetTimer(HandleDrop, this, &AMannequin::DropSec, 0.05f, false);

			bUseControllerRotationYaw = false;
			canFire = false;
			PrimaryInHand = false;
			WeaponCombat = false;
			PrimaryWeapon = nullptr;
		}
	}
	else {
		if (!isFiring)
		{
			SecondaryWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

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
			SecondaryWeapon->GetProjectileMovementComp()->Velocity = MadeVelocity;
			SecondaryWeapon->GetProjectileMovementComp()->Activate();

			FTimerHandle HandleDrop;
			GetWorldTimerManager().SetTimer(HandleDrop, this, &AMannequin::DropSec, 0.05f, false);

			bUseControllerRotationYaw = false;
			canFire = false;
			SecondaryInHand = false;
			WeaponCombat = false;
			SecondaryWeapon = nullptr;
		}
	}
}
void AMannequin::DropSec()
{
	CurrentWeapon->EnableAll();

	if (!PrimaryInHand && SecondaryWeapon)
	{
		AttachToHand(SecondaryWeapon);
		UE_LOG(LogTemp, Warning, TEXT("i have secondary weapon! should make that my current"));
	}
	else {
		CurrentWeapon = nullptr;

		if (!SecondaryInHand && PrimaryWeapon)
		{
			AttachToHand(PrimaryWeapon);
		}
		else {
			CurrentWeapon = nullptr;
		}
	}
}
void AMannequin::ResetError()
{
	ShouldError = false;
	ErrorMessage = "";
	GetWorldTimerManager().ClearTimer(ErrorHandle);
}

void AMannequin::PrimaryEquip()
{
	if (PrimaryWeapon== nullptr) { return; }
	
	if (CurrentWeapon)
		if (CurrentWeapon->GetIsReloading() || isFiring) { return; }

	if (SecondaryInHand)
	{
		if (PickupAttachDirect)
		{
			AttachToBack(SecondaryWeapon);
			AttachToHand(PrimaryWeapon);
		}
		else {
			AttachToBack(PrimaryWeapon);
		}
	}
	else if (NadeInHand)
		{
			AttachNadeToBack(Nade);
			AttachToHand(PrimaryWeapon);
		}
	else if (MeleeInHand)
		{
			AttachNadeToBack(Melee);
			AttachToHand(PrimaryWeapon);
		}
	else{
		AttachToHand(PrimaryWeapon);
	}
}
void AMannequin::SecondaryEquip()
{
	if (SecondaryWeapon == nullptr) { return; }

	if (PrimaryInHand)
	{
		if (PickupAttachDirect)
		{
			AttachToBack(PrimaryWeapon);
			AttachToHand(SecondaryWeapon);
		}
		else {
			AttachToBack(SecondaryWeapon);
		}
	}
	else if (NadeInHand)
	{
		AttachNadeToBack(Nade);
		AttachToHand(SecondaryWeapon);
	}
	else if (MeleeInHand)
	{
		AttachNadeToBack(Melee);
		AttachToHand(SecondaryWeapon);
	}
	else {
		AttachToHand(SecondaryWeapon);
	}
}
void AMannequin::MeleeEquip()
{
	if (Melee == nullptr) { return; }

	if (PrimaryInHand)
	{
		AttachToBack(PrimaryWeapon);
		AttachNadeToHand(Melee);
	}
	else if (SecondaryInHand)
	{
		AttachToBack(SecondaryWeapon);
		AttachNadeToHand(Melee);
	}
	else if (NadeInHand)
	{
		AttachNadeToBack(Nade);
		AttachNadeToHand(Melee);
	}
	else {
		AttachNadeToHand(Melee);
	}
}
void AMannequin::NadeEquip()
{
	if (Nade == nullptr) { return; }
	
	if (PrimaryInHand)
	{
		AttachToBack(PrimaryWeapon);
		AttachNadeToHand(Nade);
	}
	else if (SecondaryInHand)
	{
		AttachToBack(SecondaryWeapon);
		AttachNadeToHand(Nade);
	}
	else if (MeleeInHand)
	{
		AttachNadeToBack(Melee);
		AttachNadeToHand(Nade);
	}
	else {
		AttachNadeToHand(Nade);
	}
}

// Called every frame
void AMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WeaponCombat)
	{
		float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
		float NewFOV = FMath::FInterpTo(GetUsedCamera()->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
		GetUsedCamera()->SetFieldOfView(NewFOV);
	}
}
// Called to bind functionality to input
void AMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMannequin::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMannequin::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AMannequin::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AMannequin::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMannequin::BeginJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMannequin::EndJump);


	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMannequin::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMannequin::EndCrouch);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AMannequin::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AMannequin::EndZoom);

}
void AMannequin::UnPossessed()
{
	Super::UnPossessed();

	if (ensure(CurrentWeapon == nullptr)) { return; }

	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), CurrentWeapon->GetWeaponInfo().SocketName3P);
}
void AMannequin::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);
	EndJump();
}

void AMannequin::MoveForward(float Value)
{
	
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}
void AMannequin::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMannequin::BeginJump()
{
	Jump();
	bWantsToJump = true;
}
void AMannequin::EndJump()
{
	StopJumping();
	bWantsToJump = false;
}

void AMannequin::BeginCrouch()
{
	Crouch();
	bWantToCrouch = true;
}
void AMannequin::EndCrouch()
{
	UnCrouch();
	bWantToCrouch = false;
}

void AMannequin::BeginZoom()
{
	bWantsToZoom = true;
}
void AMannequin::EndZoom()
{
	bWantsToZoom = false;
}

void AMannequin::PullTrigger()
{
	if (!ensure(CurrentWeapon) && !canFire) { return; }
	CurrentWeapon->OnFire();
}
void AMannequin::StartFire()
{
	if (NadeInHand)
	{
		Nade->ThrowNade();
	}
	else if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
		isFiring = true;
	}
}
void AMannequin::EndFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->EndFire();
		isFiring = false;
	}
}

void AMannequin::SetCanPickup(bool boolcanpickup)
{
	CanPickup = boolcanpickup;
}
void AMannequin::SetNadeThrown(UAnimMontage* NadeBasePose)
{
	GetMesh()->GetAnimInstance()->Montage_Stop(0.05f, NadeBasePose);
	bUseControllerRotationYaw = false;;
	NadeInHand = false;
	AmountOfNade = AmountOfNade - 1;
	if (AmountOfNade <= 0)
	{
		Nade = nullptr;
	}
	else if (NadeActor)
	{
			Nade = GetWorld()->SpawnActor<ANadeActor>(NadeActor);
			Nade->SetOwner(this);
			NadeEquip();
	}
}

void AMannequin::GetWeapons(AWeaponBase*& CurrentWeaponOut, AWeaponBase*& PrimaryWeaponOut, AWeaponBase*& SecondaryWeaponOut, ANadeActor*& NadeOut, AMeleeActor*& MeleeOut)
{
	if (CurrentWeapon)
		CurrentWeaponOut = CurrentWeapon;
	if (PrimaryWeapon)
		PrimaryWeaponOut = PrimaryWeapon;
	if (SecondaryWeapon)
		SecondaryWeaponOut = SecondaryWeapon;
	if (Nade)
		NadeOut = Nade;
	if (Melee)
		MeleeOut = Melee;
}
UCameraComponent* AMannequin::GetUsedCamera()
{
	return IsFirstPerson ? FP_Camera : TP_Camera;
}
bool AMannequin::GetWeaponCombat()
{
	return WeaponCombat;
}
bool AMannequin::GetIsFirstPerson()
{
	return IsFirstPerson;
}
void AMannequin::GetErrorMessage(bool& ShouldErrorOut, FString& ErrorMessageOut)
{
	ShouldErrorOut = ShouldError;
	ErrorMessageOut = ErrorMessage;
}