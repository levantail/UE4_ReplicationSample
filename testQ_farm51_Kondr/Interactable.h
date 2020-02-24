// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Abstract_Interact.h"

#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS(Blueprintable)
class TESTQ_FARM51_KONDR_API AInteractable : public AAbstract_Interact
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractable();

	// Played when player interract with current actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* SoundInt;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
