// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/GameInstance.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "GameLiftTutorialGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API UGameLiftTutorialGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UGameLiftTutorialGameInstance();

	virtual void Shutdown() override;

	//want to set a timer that periodically calls the aws service endpoint in order to receive ping values
	virtual void Init() override;
	UPROPERTY()
		FString AccessToken;

	UPROPERTY()
		FString IdToken;

	UPROPERTY()
		FString RefreshToken;

	UPROPERTY()
		FTimerHandle RetrieveNewTokensHandle;
	//ping
	UPROPERTY()
		FTimerHandle GetResponseTImeHandle;

	//want to keep track of more then one response time results
	TDoubleLinkedList<float> PlayerLatencies; //can use this as a queue to keep track of the most recent pings
	
	//Set by the MatchmakingWidget class
	FString MatchmakingTicketId;

	UFUNCTION()
		void SetCognitoTokens(FString NewAccessToken, FString NewIDToken, FString NewRefreshToken);

private:
	FHttpModule* HttpModule;

	UPROPERTY()
		FString ApiUrl;

	UPROPERTY()
		FString RegionCode;

	UFUNCTION()
		void RetrieveNewTokens();

	UFUNCTION()
		void GetResponseTime();
	

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnGetResponseTimeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
