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

	UPROPERTY()
		FString AccessToken;

	UPROPERTY()
		FString IdToken;

	UPROPERTY()
		FString RefreshToken;

	UPROPERTY()
		FTimerHandle RetrieveNewTokensHandle;

	UFUNCTION()
		void SetCognitoTokens(FString NewAccessToken, FString NewIDToken, FString NewRefreshToken);

private:
	FHttpModule* HttpModule;

	UPROPERTY()
		FString ApiUrl;

	UFUNCTION()
		void RetrieveNewTokens();

	void OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
};
