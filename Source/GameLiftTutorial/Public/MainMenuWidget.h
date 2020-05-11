// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextReaderComponent.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	bool FailedToJoinGame;
	bool SucceededToJoinGame;

	UPROPERTY()
		UTextReaderComponent* TextReader;

private:
	FString AccessKey;
	FString SecretKey;
	FString QueueName;
};
