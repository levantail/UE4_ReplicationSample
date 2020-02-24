// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"

// Sets default values
APickUpItem::APickUpItem()
{
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

	CollisionBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBoxComp->SetupAttachment(RootComponent);
	CollisionBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	FItemInventoryColor = FSlateColor(FLinearColor::White);

	bReplicates = true;
	bReplicateMovement = true;

	if (Role == ROLE_Authority) {
		bIsVisible = true;
		bIsHudDisplay = true;
	}
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickUpItem::Client_PlayInteractSound_Implementation(USoundBase* SoundToPlay)
{
	if (Role == ROLE_SimulatedProxy) {
		if (PickUpSound != nullptr && SoundAttenuation != nullptr) {
				UGameplayStatics::PlaySoundAtLocation(
					this,
					SoundToPlay, GetActorLocation(),
					1,
					1,
					0,
					SoundAttenuation);
		}
	}
}

// Update actor location for newly joined player or which step's inside the net radius.
void APickUpItem::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	if (!GetActorLocation().Equals(ReplicatedMovement.Location, 1.0f)) {
		SetActorLocation(ReplicatedMovement.Location, false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (!GetActorRotation().Equals(ReplicatedMovement.Rotation, 1.0f)) {
		SetActorRotation(ReplicatedMovement.Rotation, ETeleportType::TeleportPhysics);
	}
}

void APickUpItem::SERVER_DropItemAtLocation_Implementation(FVector DropLocation)
{
	SetActorLocation(DropLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

bool APickUpItem::SERVER_DropItemAtLocation_Validate(FVector DropLocation)
{
	return true;
}

void APickUpItem::SetItemInventoryColor(FSlateColor NewSlateColor)
{
	FItemInventoryColor = NewSlateColor;
}

void APickUpItem::SetVisibilityToWorld(bool bNewVisible)
{
	if (Role == ROLE_Authority) {
		bIsVisible = bNewVisible;
		//Update for server
		UpdateVisibility();
	}
}

void APickUpItem::OnRep_bIsActivated()
{
	//UpdateForClient
	UpdateVisibility();
}

void APickUpItem::UpdateVisibility()
{
	ECollisionEnabled::Type collision = bIsVisible ?
		ECollisionEnabled::QueryAndPhysics :
		ECollisionEnabled::NoCollision;

	GetStaticMeshComponent()->SetVisibility(bIsVisible);
	GetStaticMeshComponent()->SetSimulatePhysics(bIsVisible);
	GetStaticMeshComponent()->SetCollisionEnabled(collision);

	if (!bIsVisible == true) {
		Client_PlayInteractSound(PickUpSound);
		// Engine bug ? Force to immediately update mesh location to avoid "bug" when drop
		// location is similar to previous pickup location
		// I Suppose that physics in multi-player games works in different way, and this is not
		// a proper way to do it.
		SetActorLocation(FVector(0, 0, -1000), false, nullptr, ETeleportType::TeleportPhysics);
	}
	else {
		Client_PlayInteractSound(DropSound);
	}
	CollisionBoxComp->SetCollisionEnabled(collision);
}

void APickUpItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpItem, bIsVisible);
}
