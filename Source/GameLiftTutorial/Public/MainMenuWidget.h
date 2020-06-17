// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "MainMenuWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoginCompleteEvent);
class UWebBrowser;
/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);
	//The "BeginPlay" but for widget classes

	//get latency from GameInstance class and calculate ping
	UPROPERTY()
		FTimerHandle SetAveragePlayerLatencyHandle;

	UPROPERTY(BlueprintAssignable)
		FOnLoginCompleteEvent OnLoginComplete;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	FHttpModule* HttpModule;
	
	UPROPERTY()
		FString LoginUrl;

	UPROPERTY()
		FString ApiUrl;

	UPROPERTY()
		FString CallbackUrl;

	UPROPERTY()	
		UWebBrowser* WebBrowser;
	UPROPERTY()
		float AveragePlayerLatency;

	UFUNCTION()
		void HandleLoginUrlChange();

	UFUNCTION()
		void SetAveragePlayerLatency();
	
	void OnExchangeCodeForTokensResponseRecieved(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//Get player wins losses
	void OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	
};
