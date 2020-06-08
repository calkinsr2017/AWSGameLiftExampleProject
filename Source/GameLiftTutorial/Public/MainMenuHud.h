// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHud.generated.h"

/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API AMainMenuHud : public AHUD
{
	GENERATED_BODY()
public:
	AMainMenuHud();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
		TSubclassOf<UUserWidget> MainMenuWidgetClass;
};
