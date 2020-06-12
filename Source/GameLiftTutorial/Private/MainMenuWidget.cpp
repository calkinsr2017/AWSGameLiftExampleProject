// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "TextReaderComponent.h"
#include "WebBrowser.h"
#include "WebBrowserModule.h"
#include "IWebBrowserSingleton.h"
#include "IWebBrowserCookieManager.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "GameLiftTutorialGameInstance.h"
#include "TimerManager.h"
#include "Engine/World.h"

//This tutorial assumes we always have valid access tokens

UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	LoginUrl = TextReader->ReadFile("Urls/LoginUrl.txt");
	ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");
	CallbackUrl = TextReader->ReadFile("Urls/CallbackUrl.txt");

	HttpModule = &FHttpModule::Get();

	//default for player latency
	AveragePlayerLatency = 60.0;
}

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;

	//Widgets
	WebBrowser = (UWebBrowser*)GetWidgetFromName(TEXT("WebBrowser_Login"));

	GetWorld()->GetTimerManager().SetTimer(SetAveragePlayerLatencyHandle, this, &UMainMenuWidget::SetAveragePlayerLatency, 1.0f, true, 1.0f);
	IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	
	if (WebBrowserSingleton != nullptr)
	{
		TOptional<FString> DefaultContext;
		TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);
		if (CookieManager.IsValid())
		{
			CookieManager->DeleteCookies();
		}
	}

	WebBrowser->LoadURL(LoginUrl);

	FScriptDelegate LoginDelegate;
	LoginDelegate.BindUFunction(this, "HandleLoginUrlChange");
	WebBrowser->OnUrlChanged.Add(LoginDelegate);
	
	
}

void UMainMenuWidget::HandleLoginUrlChange()
{
	FString BrowserUrl = WebBrowser->GetUrl();
	FString Url;
	FString QueryParameters;
	if (BrowserUrl.Split("?", &Url, &QueryParameters))
	{
		if (Url.Equals(CallbackUrl))
		{
			FString ParameterName;
			FString ParameterValue;

			if (QueryParameters.Split("=", &ParameterName, &ParameterValue))
			{
				if(ParameterName.Equals("code"))
				{
					ParameterValue = ParameterValue.Replace(*FString("#"), *FString(""));

					TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
					RequestObj->SetStringField(ParameterName, ParameterValue);
					FString RequestBody;
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
					if(FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer))
					{
						TSharedRef<IHttpRequest> ExchangeCodeForTokensRequest = HttpModule->CreateRequest();
						ExchangeCodeForTokensRequest->OnProcessRequestComplete().BindUObject(this, &UMainMenuWidget::OnExchangeCodeForTokensResponseRecieved);

						ExchangeCodeForTokensRequest->SetURL(ApiUrl + "/exchangecodefortokens");
						ExchangeCodeForTokensRequest->SetVerb("POST");
						ExchangeCodeForTokensRequest->SetHeader("Content-Type", "application/json");
						ExchangeCodeForTokensRequest->SetContentAsString(RequestBody);
						ExchangeCodeForTokensRequest->ProcessRequest();
						
					}
				}
			}
		}
	}
}

void UMainMenuWidget::OnExchangeCodeForTokensResponseRecieved(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader,JsonObject))
		{
			if(JsonObject->HasField(("access_token")) && JsonObject->HasField(("id_token")) && JsonObject->HasField(("refresh_token")))
			{
				UGameInstance* GameInstance = GetGameInstance();

				if(GameInstance != nullptr)
				{
					UGameLiftTutorialGameInstance* GameLiftTutorialGameInstance = Cast<UGameLiftTutorialGameInstance>(GameInstance);
					if(GameLiftTutorialGameInstance != nullptr)
					{
						FString AccessToken = JsonObject->GetStringField("access_token");
						FString IdToken = JsonObject->GetStringField("id_token");
						FString RefreshToken = JsonObject->GetStringField("refresh_token");
						//Pass the tokens for use with the AWS console
						GameLiftTutorialGameInstance->SetCognitoTokens(AccessToken, IdToken, RefreshToken);

						//Get player win loss data if we would like to add this functionality to our widgets
						TSharedRef<IHttpRequest> GetPlayerDataRequest = HttpModule->CreateRequest();
						GetPlayerDataRequest->OnProcessRequestComplete().BindUObject(this, &UMainMenuWidget::OnGetPlayerDataResponseReceived);
						GetPlayerDataRequest->SetURL(ApiUrl + "/getplayerdata");
						GetPlayerDataRequest->SetVerb("GET");
						GetPlayerDataRequest->SetHeader("Authorization", AccessToken);
						GetPlayerDataRequest->SetHeader("Content-Type", "application/json");
						GetPlayerDataRequest->ProcessRequest();
						
						//Call to switch widgets once used
						OnLoginComplete.Broadcast();

					}
				}
			}
		}
	}
}

//Used above to get win loss data. Can copy paste this function to other class if we want. Can update widget values with Wins, Losses within function
void UMainMenuWidget::OnGetPlayerDataResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
		if(FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			if(JsonObject->HasField("playerData"))
			{
				TSharedPtr<FJsonObject> PlayerData = JsonObject->GetObjectField("PlayerData");
				TSharedPtr<FJsonObject> WinsObject = PlayerData->GetObjectField("Wins");
				TSharedPtr<FJsonObject> LossesObject = PlayerData->GetObjectField("Losses");

				FString Wins = WinsObject->GetStringField("N");
				FString Losses = LossesObject->GetStringField("N");

				//Update textboxes here
			}
		}
	}
}

//get player latency from get instance
void UMainMenuWidget::SetAveragePlayerLatency()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr)
	{
		UGameLiftTutorialGameInstance* GameLiftTutorialGameInstance = Cast<UGameLiftTutorialGameInstance>(GameInstance);
		if (GameLiftTutorialGameInstance != nullptr)
		{
			float TotalPlayerLatency = 0.0f;
			for(float i: GameLiftTutorialGameInstance->PlayerLatencies)
			{
				TotalPlayerLatency += i;
			}

			if (TotalPlayerLatency > 0)
			{
				AveragePlayerLatency = TotalPlayerLatency / GameLiftTutorialGameInstance->PlayerLatencies.Num();

				//now add averagePlayerLatency to a text box or whatever else. We can round this value to int
				FString PingString = "Ping:" + FString::FromInt(FMath::RoundToInt(AveragePlayerLatency)) + "ms";
			}
		}
	}
}


