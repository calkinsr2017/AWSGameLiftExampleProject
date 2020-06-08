// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHud.h"
#include "UObject/ConstructorHelpers.h"

AMainMenuHud::AMainMenuHud()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuObj(TEXT("/Game/UI/Widgets/UI_MainMenu"));
	MainMenuWidgetClass = MainMenuObj.Class;
}

void AMainMenuHud::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if(PlayerController != nullptr)
	{
		PlayerController -> bShowMouseCursor = true;
	}

	if(MainMenuWidgetClass != nullptr)
	{
		UUserWidget* MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if(MainMenuWidget != nullptr)
		{
			MainMenuWidget->AddToViewport();
			MainMenuWidget->SetFocus();
		}
	}
}


