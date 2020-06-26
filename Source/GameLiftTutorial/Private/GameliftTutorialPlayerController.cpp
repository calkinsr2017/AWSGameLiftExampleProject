// Fill out your copyright notice in the Description page of Project Settings.


#include "GameliftTutorialPlayerController.h"

#include "Kismet/GameplayStatics.h"

AGameliftTutorialPlayerController::AGameliftTutorialPlayerController()
{
	
}

void AGameliftTutorialPlayerController::SetupPlayerInput()
{
	Super::SetupInputComponent(InputComponent);

	
}

void AGameliftTutorialPlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}
