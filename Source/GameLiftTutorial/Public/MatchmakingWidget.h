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
	void OnStartMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); //updated 2

public:
	UFUNCTION(BlueprintCallable)
		void CancelMatchmaking();

	UPROPERTY()
		FTimerHandle SetAveragePlayerLatencyHandle;

protected:
	void OnStopMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful); //updated 3
	
private:
	//Called every 10 seconds by the AWS server
	void PollMatchmaking();

	UPROPERTY()
		bool SearchingForGame;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	void OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/* ================================== VARIABLES ========================== */
protected:
	FString MatchmakingTicketId;
	FString AccessToken;
	FHttpModule* HttpModule;
	
	UPROPERTY()
		FTimerHandle PollMatchmakingHandle;

private:
	FString StartMatchLookupUrl;
	FString CancelMatchLookupUrl;
	FString PollMatchmakingUrl;

	UPROPERTY()
		FString RegionCode;

	UFUNCTION()
		void SetAveragePlayerLatency();

	UPROPERTY()
		float AveragePlayerLatency;
};
