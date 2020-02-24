// Fill out your copyright notice in the Description page of Project Settings.


#include "Abstract_Interact.h"

// Sets default values
AAbstract_Interact::AAbstract_Interact()
{
	PrimaryActorTick.bCanEverTick = false;

	HUD_DisplayAction = FString("DisplayActionString");
	HUD_DisplayObjectName = FString("ObjectNameString");

	if (ROLE_Authority) {
		MaxHealth = 100;
		CurrentHealth = MaxHealth;
		bIsHudDisplay = false;
		bIsInvulnerable = true;
	}
}

void AAbstract_Interact::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAbstract_Interact, bIsHudDisplay);
	DOREPLIFETIME(AAbstract_Interact, bIsInvulnerable);
	DOREPLIFETIME(AAbstract_Interact, CurrentHealth);
}

// Called when the game starts or when spawned
void AAbstract_Interact::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAbstract_Interact::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AAbstract_Interact::OnHealthUpdate()
{
	//Server-specific functionality
	if (Role == ROLE_Authority) {
		if (CurrentHealth <= 0) {
			Destroy();
		}
	}
}

void AAbstract_Interact::Client_PlayLocalSound_Implementation(float DamageApplied)
{
	if (Role == ROLE_SimulatedProxy) {
		if (DamageSound != nullptr && DamageApplied > 0) {
			UGameplayStatics::PlaySoundAtLocation(this, DamageSound, GetActorLocation(), 1, 1, 0, SoundAttenuation);
		}
		else if (DestroySound != nullptr && DamageApplied <= 0) {
			UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation(), 1, 1, 0, SoundAttenuation);
		}
	}
}

void AAbstract_Interact::SetCurrentHealth(float healthValue)
{
	if (Role == ROLE_Authority) {
		if (healthValue == CurrentHealth) {
			return;
		}

		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AAbstract_Interact::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsInvulnerable == true) {
		return 0.0f;
	}

	float damageApplied = CurrentHealth - DamageTaken;

	Client_PlayLocalSound(damageApplied);

	SetCurrentHealth(damageApplied);

	return damageApplied;
}

// Called every frame
void AAbstract_Interact::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAbstract_Interact::SetbIsHudDisplay(bool bNewbIsHudDisplay)
{
	if (Role == ROLE_Authority) {
		bIsHudDisplay = bNewbIsHudDisplay;
	}
}

void AAbstract_Interact::SetbIsInvulnerable(bool bNewIsInvulnerable)
{
	if (Role == ROLE_Authority) {
		bIsInvulnerable = bNewIsInvulnerable;
	}
}

void AAbstract_Interact::SetHUD_DisplayAction(FString NewString)
{
	HUD_DisplayAction = NewString;
}

void AAbstract_Interact::SetHUD_DisplayObjectName(FString NewString)
{
	HUD_DisplayObjectName = NewString;
}
