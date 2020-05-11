// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextReaderComponent.h"
#include "GameLiftClientObject.h"
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
	void NativeConstruct();
	void CreatePlayerSession(const FString& GameSessionId, const FString& PlayerSessionId);

	UFUNCTION(BlueprintCallable, Category = "GameLift")
		void JoinGame();
	void StartGameSessionPlacement(const FString& QueueNameInput, const int& MaxPlayerCount,
	                               const FString& PlacementId);

protected:
	void OnCreatePlayerSessionSuccess(const FString& IPAddress, const FString& Port, const FString& PlayerSessionID);
	void OnCreatePlayerSessionFailed(const FString& ErrorMessage);
	FString GenerateRandomId();

protected:
	bool FailedToJoinGame;
	bool SucceededToJoinGame;
	bool AttemptToJoinGameFinished;


	UPROPERTY()
		UTextReaderComponent* TextReader;
	UPROPERTY()
		UGameLiftClientObject* Client;

private:
	FString AccessKey;
	FString SecretKey;
	FString QueueName;
};
