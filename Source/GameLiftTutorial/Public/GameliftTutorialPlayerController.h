// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameliftTutorialPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API AGameliftTutorialPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGameliftTutorialPlayerController();

	UFUNCTION(BlueprintCallable)
		void QuitGame();
};
