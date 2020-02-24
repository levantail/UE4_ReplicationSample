// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include <testQ_farm51_Kondr\Abstract_Interact.h>


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CollisionQueryParams.h"

// ?
//#include "Containers/Array.h"
#include <testQ_farm51_Kondr\PickUpItem.h>
#include <testQ_farm51_Kondr\Interactable.h>

#include "testQ_farm51_KondrCharacter.generated.h"

class UInputComponent;


//Didn't optimized for network purposes (data base required)?
// struct for Single inventory item
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_USTRUCT_BODY()
		public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class APickUpItem* Item;
};

UCLASS(config=Game)
class AtestQ_farm51_KondrCharacter : public ACharacter
{
	GENERATED_BODY()
		/** Pawn mesh: 1st person view (arms; seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;

	/** Pawn mesh (seen only by other players) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1POther;

	/** Gun mesh (seen only by other players) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* FP_GunOther;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;
	//Standard sound attenuation
	UPROPERTY(EditDefaultsOnly, Category = Sound)
		class USoundAttenuation* SoundAttenuation;


public:
	AtestQ_farm51_KondrCharacter();

	virtual void Tick(float DeltaTime) override;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;
		/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;
	/** Projectile class to spawn */


protected:

	virtual void BeginPlay();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//SERVER SIDE
	//Try to use/pickup object
	UFUNCTION(Reliable, Server)
		void SERVER_Use();

	//Drop item from inventory
	UFUNCTION(Reliable, Server)
		void SERVER_DropItem(int itemToDrop);


	// Inventory items
	UPROPERTY(ReplicatedUsing = OnRep_PickupItemsUpdate, VisibleDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
		TArray <FInventoryItem> PickupItems;
	UFUNCTION()
		virtual void OnRep_PickupItemsUpdate();

	// Maximum inventory items which player could care
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int MaxInventoryItems;

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	//SHOOTING
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AtestQ_farm51_KondrProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		bool bIsFiringWeapon;
	UPROPERTY()
		FTimerHandle FiringTimer;

	// Attempt to start fire
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopFire();
	UFUNCTION(Server, Reliable)
		void SERVER_HandleFire();


public:
	// SETTERS GETTERS
	UFUNCTION(BlueprintCallable, Category = Gameplay)
		bool PickUpItemToInventory(class APickUpItem* NewItem);
	UFUNCTION(BlueprintCallable, Category = Gameplay)
		bool DropItemFromInventory(int itemToDrop);
	
	//EVENTS
	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void Event_Use();
	UFUNCTION(BlueprintCallable, Category = Gameplay)
		void Event_DropItem();


	//FOR BLUEPRINT UMG
	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent, meta = (DisplayName = "Set Interact Widget"))
		void Event_SetInteractWidget(const FString& DisplayInteractKey_out, const FString& DisplayActionName_out, const FString& DisplayObjectName_out);

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent, meta = (DisplayName = "Remove Interact Widget"))
		void Event_RemoveInteractWidget();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent, meta = (DisplayName = "Update Inventory"))
		void Event_UpdateInventoryWidget();

	UFUNCTION(BlueprintCosmetic, BlueprintImplementableEvent, meta = (DisplayName = "Get Selected Item Widget"))
		int Event_GetSelectedItemWidget();

	// WIDGETS 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UUserWidget* InventoryWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UUserWidget* InteractWidget;

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};