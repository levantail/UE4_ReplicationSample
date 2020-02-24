// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyPack.h"

ADestroyPack::ADestroyPack()
{
	bReplicates = true;
	bIsHudDisplay = true;
	bIsInvulnerable = false;
	bReplicateMovement = true;

	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	if (Role == ROLE_Authority) {
		CurrentActorState = EActorState::INTACT;
	}

	RotationRate = 100;
}

void ADestroyPack::SERVER_ResetActorState_Implementation()
{
	SetCurrentHealth(100);
}

void ADestroyPack::OnRep_CurrentActorState()
{
	if (Role == ROLE_SimulatedProxy) {
		switch (CurrentActorState)
		{
		case EActorState::INTACT:

			if (EffectCurrent != nullptr) {
				EffectCurrent->DestroyComponent();
				EffectCurrent = nullptr;
			}

			if (ResetSound != nullptr && SoundAttenuation != nullptr) {
				//if (UWorld* World = GetWorld())
					UGameplayStatics::PlaySoundAtLocation(this, ResetSound, GetActorLocation(), 1, 1, 0, SoundAttenuation);
			}
			break;
		case EActorState::DAMAGED:
			if (EffectDamaged != nullptr) {
				if (EffectCurrent != nullptr) {
					EffectCurrent->DestroyComponent();
					EffectCurrent = nullptr;
				}
				EffectCurrent = UGameplayStatics::SpawnEmitterAttached(EffectDamaged, RootComponent, FName(""), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
			}

			break;
		case EActorState::BROKEN:
			if (EffectBroken != nullptr) {
				if (EffectCurrent != nullptr) {
					EffectCurrent->DestroyComponent();
					EffectCurrent = nullptr;
				}
				EffectCurrent = UGameplayStatics::SpawnEmitterAttached(EffectBroken, RootComponent, FName(""), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
			}
			break;
		}
	}
}

// Update actor location for newly joined player or which step's inside the net radius.
void ADestroyPack::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	if (!GetActorLocation().Equals(ReplicatedMovement.Location, 1.0f)) {
		SetActorLocation(ReplicatedMovement.Location, false, nullptr, ETeleportType::TeleportPhysics);
	}

	if (!GetActorRotation().Equals(ReplicatedMovement.Rotation, 1.0f)) {
		SetActorRotation(ReplicatedMovement.Rotation, ETeleportType::TeleportPhysics);
	}
}

void ADestroyPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Role == ROLE_Authority && CurrentActorState != EActorState::BROKEN) {
		if (CurrentActorState == EActorState::DAMAGED) {
			FRotator NewRotator = FRotator(RotationRate / 3.f, 0.f, 0.f) * DeltaTime;
			AddActorLocalRotation(NewRotator.Quaternion());
		}
		else
		{
			FRotator NewRotator = FRotator(RotationRate, 0.f, 0.f) * DeltaTime;
			AddActorLocalRotation(NewRotator.Quaternion());
		}
	}
}

void ADestroyPack::BeginPlay()
{
	Super::BeginPlay();
	OriginTransform = GetActorTransform();
}

void ADestroyPack::OnHealthUpdate()
{
	//Server-specific functionality
	if (Role == ROLE_Authority) {
		//UE_LOG(LogTemp, Warning, TEXT("AUTHORITY DestroyPack: Health Updated: %s : %f"), *GetHUD_DisplayObjectName(), CurrentHealth);

		if (CurrentHealth >= 100) {
			if (CurrentActorState != EActorState::INTACT) {
				CurrentActorState = EActorState::INTACT;
				GetStaticMeshComponent()->SetSimulatePhysics(false);
				SetActorTransform(OriginTransform);
			}
		}
		else if (CurrentHealth <= 0) {
			if (CurrentActorState != EActorState::BROKEN) {
				CurrentActorState = EActorState::BROKEN;
				GetStaticMeshComponent()->SetSimulatePhysics(true);
			}
			//Destroy();
		}
		else {
			if (CurrentActorState != EActorState::DAMAGED) {
				CurrentActorState = EActorState::DAMAGED;
			}
		}
	}
}

void ADestroyPack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADestroyPack, CurrentActorState);
}