// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "UnrealNetwork.h"
#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Abstract_Interact.generated.h"

UCLASS(ABSTRACT)
class TESTQ_FARM51_KONDR_API AAbstract_Interact : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAbstract_Interact();

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	// Sound for damage receive
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DamageSound;
	// Sound for destroy/death actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DestroySound;
	// Standard sound attenuation for all sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		class USoundAttenuation* SoundAttenuation;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Should be hud displayed when player traced this object
	UPROPERTY(Replicated, BlueprintGetter = GetbIsHudDisplay, BlueprintSetter = SetbIsHudDisplay, EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bIsHudDisplay;

	//Do we need receive any damage ?
	UPROPERTY(Replicated, BlueprintGetter = GetbIsInvulnerable, BlueprintSetter = SetbIsInvulnerable, EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool bIsInvulnerable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
		float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
		float CurrentHealth;

	UFUNCTION()
		void OnRep_CurrentHealth();

	//When health updated, this function should be call
	UFUNCTION()
		virtual void OnHealthUpdate();

	//Cosmetic
	//Action name (e.g. Open, Close). This will be diplayed on player HUD/UI
	UPROPERTY(BlueprintGetter = GetHUD_DisplayAction, BlueprintSetter = SetHUD_DisplayAction, EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FString HUD_DisplayAction = FString("DisplayActionString");
	//Cosmetic
	//Object name in game (not a class name, e.g. apple, gun). This will be diplayed on player HUD/UI
	UPROPERTY(BlueprintGetter = GetHUD_DisplayObjectName, BlueprintSetter = SetHUD_DisplayObjectName, EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FString HUD_DisplayObjectName = FString("ObjectNameString");
	//Cosmetic
	//Play local sound at all clients, except server
	UFUNCTION(Reliable, NetMulticast)
		void Client_PlayLocalSound(float DamageApplied);

public:	
	// SETTERS GETTERS
	UFUNCTION(BlueprintGetter)
		FORCEINLINE bool GetbIsHudDisplay() const {return bIsHudDisplay;}
	UFUNCTION(BlueprintSetter)
		void SetbIsHudDisplay(bool bNewbIsHudDisplay);

	UFUNCTION(BlueprintGetter)
		FORCEINLINE bool GetbIsInvulnerable() {return bIsInvulnerable;}
	UFUNCTION(BlueprintSetter)
		void SetbIsInvulnerable(bool bNewIsInvulnerable);

	UFUNCTION(BlueprintGetter)
		FORCEINLINE FString GetHUD_DisplayAction() {return HUD_DisplayAction;}
	UFUNCTION(BlueprintSetter)
		void SetHUD_DisplayAction(FString NewString);

	UFUNCTION(BlueprintGetter)
		FORCEINLINE FString GetHUD_DisplayObjectName() {return HUD_DisplayObjectName;}
	UFUNCTION(BlueprintSetter)
		void SetHUD_DisplayObjectName(FString NewString);

	UFUNCTION(BlueprintPure, Category = Gameplay )
		FORCEINLINE float GetMaxHealth() const {return MaxHealth;}
	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float healthValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// EVENT
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void InteractClick();
};
