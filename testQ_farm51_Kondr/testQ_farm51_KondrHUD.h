// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

//#include "Engine.h"
#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "testQ_farm51_KondrHUD.generated.h"

UCLASS()
class AtestQ_farm51_KondrHUD : public AHUD
{
	GENERATED_BODY()

public:
	AtestQ_farm51_KondrHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

