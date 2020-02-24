// Fill out your copyright notice in the Description page of Project Settings.
#include "Interactable.h"
#include "components\StaticMeshComponent.h"

// Sets default values
AInteractable::AInteractable()
{
	bReplicates = true;
	bReplicateMovement = true;
	SetMobility(EComponentMobility::Movable);

	if (Role == ROLE_Authority) {
		bIsHudDisplay = true;
	}
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

