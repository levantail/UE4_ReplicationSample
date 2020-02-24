// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abstract_Interact.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "DestroyPack.generated.h"

UENUM(BlueprintType)
enum class EActorState : uint8
{
	INTACT UMETA(DisplayName = "Intact"),
	DAMAGED UMETA(DisplayName = "Damaged"),
	BROKEN UMETA(DisplayName = "Broken")
};

UCLASS()
class TESTQ_FARM51_KONDR_API ADestroyPack : public AAbstract_Interact
{
	GENERATED_BODY()

public:
	ADestroyPack();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentActorState, VisibleAnywhere, BlueprintReadOnly, Category = Enum)
		EActorState CurrentActorState;

	UFUNCTION()
	virtual void OnRep_CurrentActorState();

	//Cosmetic
	// If player reset object - play this sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* ResetSound;
	//Cosmetic
	// Appears when actor hasn't full hp
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
		class UParticleSystem* EffectDamaged;
	// Appears when actor has been broken or has 0 hp
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Effects)
		class UParticleSystem* EffectBroken;

	//Pointer for current effect. If there is new effect in use - old should be destroyed, new - pointer here.
	UPROPERTY()
	 class UParticleSystemComponent* EffectCurrent;

	//SERVER resets actor state
	UFUNCTION(Reliable, Server, BlueprintCallable)
		void SERVER_ResetActorState();

	//Origin Transform, where actor was spawned. When actor were reset - teleport to this position
	UPROPERTY()
	FTransform OriginTransform;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Gameplay)
	float RotationRate;

	virtual void BeginPlay();

	void OnHealthUpdate() override;
	//Handling new player in net radius
	void OnRep_ReplicatedMovement() override;

public:
	virtual void Tick(float DeltaTime) override;

};