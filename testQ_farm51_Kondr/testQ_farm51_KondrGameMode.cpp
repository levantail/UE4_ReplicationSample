// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "testQ_farm51_KondrGameMode.h"
#include "testQ_farm51_KondrHUD.h"
#include "testQ_farm51_KondrCharacter.h"
#include "UObject/ConstructorHelpers.h"

AtestQ_farm51_KondrGameMode::AtestQ_farm51_KondrGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AtestQ_farm51_KondrHUD::StaticClass();
}
