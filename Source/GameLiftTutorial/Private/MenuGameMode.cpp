// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode.h"
#include "UObject/ConstructorHelpers.h"


AMenuGameMode::AMenuGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> MenuPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/MainMenuPawn"));
	if (MenuPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = MenuPawnBPClass.Class;
		//Test
	}
}
