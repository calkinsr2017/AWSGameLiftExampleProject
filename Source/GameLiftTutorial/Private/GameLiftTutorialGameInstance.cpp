// Fill out your copyright notice in the Description page of Project Settings.

#include "GameLiftTutorialGameInstance.h"

#include "GameLiftTutorial.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "TextReaderComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

UGameLiftTutorialGameInstance::UGameLiftTutorialGameInstance()
{
	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");

	HttpModule = &FHttpModule::Get();

}


void UGameLiftTutorialGameInstance::Shutdown()
{
	Super::Shutdown();

	if (AccessToken.Len() > 0)
	{
		TSharedRef<IHttpRequest> InvalideTokensRequest = HttpModule->CreateRequest();
		InvalideTokensRequest->SetURL(ApiUrl + "/invalidatetokens");
		InvalideTokensRequest->SetVerb("GET");
		InvalideTokensRequest->SetHeader("Content-Type", "application/json");
		InvalideTokensRequest->SetHeader("Authorization", AccessToken);
		InvalideTokensRequest->ProcessRequest();
		
	}
}

/* ============================================== COGNITO STUFF ================================================ */
void UGameLiftTutorialGameInstance::SetCognitoTokens(FString NewAccessToken, FString NewIDToken, FString NewRefreshToken)
{
	AccessToken = NewAccessToken;
	IdToken = NewIDToken;
	RefreshToken = NewRefreshToken;

	UE_LOG(LogAWS, Log, TEXT("access token: %s"), *AccessToken);
	UE_LOG(LogAWS, Log, TEXT("refresh token: %s"), *RefreshToken);

	//TODO - Error? why is this 3300
	//Refresh every 60 minutes?
	//60seconds* 60 minutes = 3600 seconds 
	GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UGameLiftTutorialGameInstance::RetrieveNewTokens, 1.0f, false, 3300.0f);
}

void UGameLiftTutorialGameInstance::RetrieveNewTokens()
{
	 if (AccessToken.Len() >0 && RefreshToken.Len() >0)
	 {
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("refreshToken", RefreshToken);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

	 	if(FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
	 	{
			TSharedRef<IHttpRequest> RetrieveNewTokensRequest = HttpModule->CreateRequest();
			RetrieveNewTokensRequest->OnProcessRequestComplete().BindUObject(this, &UGameLiftTutorialGameInstance::OnRetrieveNewTokensResponseReceived);
			RetrieveNewTokensRequest->SetURL(ApiUrl + "/retrievenewtokens");
			RetrieveNewTokensRequest->SetVerb("POST");
	 		RetrieveNewTokensRequest->SetHeader("Content-Type", "application/json");
	 		RetrieveNewTokensRequest->SetHeader("Authorization", AccessToken);
			RetrieveNewTokensRequest->SetContentAsString(RequestBody);
			RetrieveNewTokensRequest->ProcessRequest();
	 	} else
	 	{
			GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UGameLiftTutorialGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
	 	}
	 }
}

void UGameLiftTutorialGameInstance::OnRetrieveNewTokensResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if(FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if(!JsonObject->HasField("error"))
			{
				//Succesfully received tickets
				SetCognitoTokens(JsonObject->GetStringField("accessToken"), JsonObject->GetStringField("idToken"), RefreshToken);
			} else
			{
				GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UGameLiftTutorialGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
			}
		}
	} else
	{
		GetWorld()->GetTimerManager().SetTimer(RetrieveNewTokensHandle, this, &UGameLiftTutorialGameInstance::RetrieveNewTokens, 1.0f, false, 30.0f);
	}
}
/* ======================================================================================================================================= */


