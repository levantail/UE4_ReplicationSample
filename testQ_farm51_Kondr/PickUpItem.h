// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Abstract_Interact.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

#include "GameFramework/Actor.h"
#include "PickUpItem.generated.h"

UCLASS(Blueprintable)
class TESTQ_FARM51_KONDR_API APickUpItem : public AAbstract_Interact
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickUpItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Cosmetic
	//Item inventory cell color. could be a 2d texture as well
	UPROPERTY(BlueprintGetter = GetItemInventoryColor, BlueprintSetter = SetItemInventoryColor, EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FSlateColor FItemInventoryColor;

	// Cosmetic
	// Play sound for a client.
	UFUNCTION(Reliable, Client)
	void Client_PlayInteractSound(USoundBase* SoundToPlay);

	//REPLICATED
	void OnRep_ReplicatedMovement() override;

	// Is actor visible to world
	UPROPERTY(ReplicatedUsing = OnRep_bIsActivated)
		bool bIsVisible;
	UFUNCTION()
		virtual void OnRep_bIsActivated();
	UFUNCTION()
		void UpdateVisibility();

public:
	//SERVER SIDE

	//Drop action. Called from Character. 
	UFUNCTION(Reliable, Server, WithValidation)
	void SERVER_DropItemAtLocation(FVector DropLocation);

	//SETTERS GETTERS
	UFUNCTION(BlueprintGetter)
		FORCEINLINE FSlateColor GetItemInventoryColor() {return FItemInventoryColor;}
	UFUNCTION(BlueprintSetter)
		void SetItemInventoryColor(FSlateColor NewSlateColor);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Collision)
		class UBoxComponent* CollisionBoxComp;
	// TODO create in c++ function that plays interact sound
	//Sound to play when actor was taken to inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* PickUpSound;
	//Sound to play when actor was droped from inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* DropSound;

	//Used when drop/pickup events occurs. 
	void SetVisibilityToWorld(bool bNewVisible);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};