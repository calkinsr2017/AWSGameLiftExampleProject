// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"

#include "GameliftTutorialPlayerController.h"
#include "MainMenuHud.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	HUDClass = AMainMenuHud::StaticClass();

	PlayerControllerClass = AGameliftTutorialPlayerController::StaticClass();
}
