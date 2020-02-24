// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "testQ_farm51_KondrProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AtestQ_farm51_KondrProjectile::AtestQ_farm51_KondrProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	if (Role == ROLE_Authority) {
		CollisionComp->OnComponentHit.AddDynamic(this, &AtestQ_farm51_KondrProjectile::OnProjectileImpact);
	}
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	// Set as root component
	RootComponent = CollisionComp;

	Projectile_SMComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Projectile_SMComp->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;

	DamageType = UDamageType::StaticClass();
	Damage = 20.0f;
	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	//bReplicateMovement = true;
	bReplicates = true;
}

void AtestQ_farm51_KondrProjectile::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	if (Role == ROLE_SimulatedProxy) {
		if (ExplosionEffect != nullptr) {
			UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
		}
	}
}

void AtestQ_farm51_KondrProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Role == ROLE_SimulatedProxy) {
			UGameplayStatics::PlaySoundAtLocation(this, SoundBase, GetActorLocation(), 1, 1, 0, SoundAttenuation);
	}

	if (Role == ROLE_Authority) {
		if (OtherActor)
		{
			UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, Instigator->Controller, this, DamageType);
		}
		Destroy();
	}
}
