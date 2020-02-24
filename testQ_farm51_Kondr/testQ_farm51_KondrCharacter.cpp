// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "testQ_farm51_KondrCharacter.h"
#include "testQ_farm51_KondrProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
//#include "MotionControllerComponent.h"
//#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AtestQ_farm51_KondrCharacter

AtestQ_farm51_KondrCharacter::AtestQ_farm51_KondrCharacter()
{

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	Mesh1POther = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1POther"));
	Mesh1POther->SetOwnerNoSee(true);
	Mesh1POther->SetupAttachment(RootComponent);

	FP_GunOther = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_GunOther"));
	FP_GunOther->SetOwnerNoSee(true);
	FP_GunOther->SetupAttachment(Mesh1POther, TEXT("GunGripPoint"));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
	MaxInventoryItems = 4;

	//ProjectileClass = AtestQ_farm51_KondrProjectile::StaticClass();
	FireRate = 0.25f;
	bIsFiringWeapon = false;

	bReplicateMovement = true;
	bReplicates = true;
	
}

void AtestQ_farm51_KondrCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AtestQ_farm51_KondrCharacter,PickupItems);

}

void AtestQ_farm51_KondrCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

}

void AtestQ_farm51_KondrCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_AutonomousProxy) {
		UWorld* World = GetWorld();

		if (!World)
			return;

		//UE_LOG(LogTemp, Warning, TEXT("ROT: %s"), *GetBaseAimRotation().Vector().ToString());

		FVector Start = FirstPersonCameraComponent->GetComponentLocation();
		FVector End = Start + (GetBaseAimRotation().Vector() * 200.0f);

		//DrawDebugLine(World, Start, End, FColor::Yellow, false, 5.f);
		//DrawDebugSphere(World, Start, 3, 1, FColor::Cyan, false, 5.f);
		//DrawDebugSphere(World, End, 3, 1, FColor::Red, false, 5.f);

		FHitResult HitResult(EForceInit::ForceInit);

		//Ignore Self
		FCollisionQueryParams TraceParams(
			FName(TEXT("TraceInteractive")),
			false,
			this);

		World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Camera, TraceParams);
		if (HitResult.GetActor() != nullptr) {

			AAbstract_Interact* IntActor = Cast<AAbstract_Interact>(HitResult.GetActor());
			if (IntActor != nullptr) {
				FString DisplayAction = IntActor->GetHUD_DisplayAction();
				FString DisplayObjectName = IntActor->GetHUD_DisplayObjectName();
				if (IntActor->GetbIsHudDisplay() == true) {
					Event_SetInteractWidget(FString("F"), DisplayAction, DisplayObjectName);
				}
			}
			else {
				Event_RemoveInteractWidget();
			}
		}
		else
		{
			Event_RemoveInteractWidget();
		}
	}
}
void AtestQ_farm51_KondrCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AtestQ_farm51_KondrCharacter::Event_Use);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AtestQ_farm51_KondrCharacter::Event_DropItem);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AtestQ_farm51_KondrCharacter::StartFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AtestQ_farm51_KondrCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AtestQ_farm51_KondrCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AtestQ_farm51_KondrCharacter::StartFire()
{
	if (!bIsFiringWeapon) {
		bIsFiringWeapon = true;
		if (UWorld* World = GetWorld()) {
			World->GetTimerManager().SetTimer(FiringTimer, this, &AtestQ_farm51_KondrCharacter::StopFire, FireRate, false);
			
			SERVER_HandleFire();
			
			if (Role == ROLE_AutonomousProxy) {
				if (FireSound != nullptr && SoundAttenuation != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), 1, 1, 0, SoundAttenuation);
				}

				// try and play a firing animation if specified
				if (FireAnimation != nullptr)
				{
					// Get the animation object for the arms mesh
					UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
					if (AnimInstance != nullptr)
					{
						AnimInstance->Montage_Play(FireAnimation, 1.f);
					}
				}
			}
		}
	}
}

void AtestQ_farm51_KondrCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void AtestQ_farm51_KondrCharacter::SERVER_HandleFire_Implementation()
{
	
	const FRotator SpawnRotation = GetControlRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
	//Set Spawn Collision Handling Override

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = Instigator;
	spawnParameters.Owner = this;

	if (UWorld* World = GetWorld()) {
		if (ProjectileClass != nullptr) {
			AtestQ_farm51_KondrProjectile* spawnedProjectile = GetWorld()->SpawnActor<AtestQ_farm51_KondrProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, spawnParameters);
		}
	}
}

void AtestQ_farm51_KondrCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AtestQ_farm51_KondrCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AtestQ_farm51_KondrCharacter::Event_Use()
{
	SERVER_Use();
}

void AtestQ_farm51_KondrCharacter::SERVER_Use_Implementation()
{
	if (Role == ROLE_Authority) {
		UWorld* World = GetWorld();

		if (!World)
			return;

		FVector Start = FirstPersonCameraComponent->GetComponentLocation();
		FVector End = Start + (GetBaseAimRotation().Vector() * 200.0f);

		//DrawDebugLine(World, Start, End, FColor::Yellow, false, 5.f);
		//DrawDebugSphere(World, Start, 3, 1, FColor::Cyan, false, 5.f);
		//DrawDebugSphere(World, End, 3, 1, FColor::Red, false, 5.f);

		FHitResult HitResult(EForceInit::ForceInit);

		//Ignore Self
		FCollisionQueryParams TraceParams(
			FName(TEXT("TraceInteractive")),
			false,
			this);

		World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Camera, TraceParams);

		if (HitResult.GetActor() != nullptr) {

			AAbstract_Interact* InteractActor = Cast<AAbstract_Interact>(HitResult.GetActor());
			if (InteractActor != nullptr) {
				InteractActor->InteractClick();
			}

			APickUpItem* PickUpActor = Cast<APickUpItem>(HitResult.GetActor());
			if (PickUpActor != nullptr) {
				//PickUpActor->InteractClick();
				PickUpItemToInventory(PickUpActor);
				Event_UpdateInventoryWidget();
			}
		}
	}
}

bool AtestQ_farm51_KondrCharacter::PickUpItemToInventory(class APickUpItem* NewItem)
{
	if (PickupItems.Num() < MaxInventoryItems) {
		FInventoryItem invWrapper;
		invWrapper.Item = NewItem;
		PickupItems.Add(invWrapper);
		NewItem->SetVisibilityToWorld(false);
		return true;
	}
	return false;
}

bool AtestQ_farm51_KondrCharacter::DropItemFromInventory(int itemToDrop)
{
	if (PickupItems.Num() == 0 && PickupItems.Num()-1 <= itemToDrop)
		return false;

	FVector FDropLocation = GetActorLocation() + FVector (0,0, 10) +
		(GetActorForwardVector() * 150.f);

	PickupItems[itemToDrop].Item->SERVER_DropItemAtLocation(FDropLocation);
	PickupItems[itemToDrop].Item->SetVisibilityToWorld(true);
	PickupItems.RemoveAt(itemToDrop);

	return true;
}

void AtestQ_farm51_KondrCharacter::Event_DropItem()
{
	int itemToDrop = Event_GetSelectedItemWidget();
	if (itemToDrop == NULL) {
		itemToDrop = 0;
	}

	if (itemToDrop != -1)
		SERVER_DropItem(itemToDrop);
}

void AtestQ_farm51_KondrCharacter::OnRep_PickupItemsUpdate()
{
	if (Role == ROLE_AutonomousProxy) {
		Event_UpdateInventoryWidget();
	}
}

void AtestQ_farm51_KondrCharacter::SERVER_DropItem_Implementation(int itemToDrop)
{
	if (Role == ROLE_Authority) {
		DropItemFromInventory(itemToDrop);
	}
}
