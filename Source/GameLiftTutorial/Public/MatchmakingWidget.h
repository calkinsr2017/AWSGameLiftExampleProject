// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Blueprint/UserWidget.h"
#include "MatchmakingWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API UMatchmakingWidget : public UUserWidget
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable)
		void BeginMatchmaking(int32 InNumPlayers);

	//Fetches the current tokens from the GameInstance to see if we have to reset them
	void FetchCurrentTokenStatus();
	
protected:
	void OnInitiateMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

public:
	UFUNCTION(BlueprintCallable)
		void CancelMatchmaking();

protected:
	void OnCancelMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
private:
	//Called every 10 seconds by the AWS server
	void PollMatchmaking();

protected:
	void OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/* ================================== VARIABLES ========================== */
protected:
	FString MatchmakingTicketId;
	FString AccessToken;
	FHttpModule* HttpModule;
	
	bool bSearchingForGame;
	FTimerHandle PollMatchmakingHandle;

private:
	FString LookForMatchUrl;
	FString CancelMatchLookupUrl;
	FString PollMatchmakingUrl;
};
