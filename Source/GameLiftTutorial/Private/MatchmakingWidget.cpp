// Fill out your copyright notice in the Description page of Project Settings.
#include "MatchmakingWidget.h"
#include "GameLiftTutorial.h"
#include "GameLiftTutorialGameInstance.h"
#include "TextReaderComponent.h"
#include "TimerManager.h"
#include "Components/TextBlock.h"
//#include "Dom/JsonObject.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Json.h"
#include "JsonUtilities.h"

UMatchmakingWidget::UMatchmakingWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	UTextReaderComponent* TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	FString ApiUrl = TextReader->ReadFile("Urls/ApiUrl.txt");

	LookForMatchUrl = ApiUrl + "/startmatchmaking";
	CancelMatchLookupUrl = ApiUrl + "/stopmatchmakingmike";
	PollMatchmakingUrl = ApiUrl + "/pollmatchmaking";

	HttpModule = &FHttpModule::Get();
	SearchingForGame = false;
}

void UMatchmakingWidget::BeginMatchmaking(int32 InNumPlayers)
{
	//TODO - Get ID from google cognito and log into AWS service with player ID

	if (!SearchingForGame)
	{

		UGameLiftTutorialGameInstance* GLGI = Cast<UGameLiftTutorialGameInstance>(GetGameInstance());
		MatchmakingTicketId = GLGI->MatchmakingTicketId;
		AccessToken = GLGI->AccessToken;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Got into Begin matchmaking");

		//Reset variables here related to searching
		SearchingForGame = false;

		if (AccessToken.Len() > 0)
		{
			//Initiate Matchmaking request
			TSharedRef<IHttpRequest> InitiateMatchmakingRequest = HttpModule->CreateRequest();
			InitiateMatchmakingRequest->OnProcessRequestComplete().BindUObject(this, &UMatchmakingWidget::OnInitiateMatchmakingResponseReceived);
			InitiateMatchmakingRequest->SetURL(LookForMatchUrl);
			InitiateMatchmakingRequest->SetVerb("GET");
			InitiateMatchmakingRequest->SetHeader("Authorization", AccessToken);
			InitiateMatchmakingRequest->ProcessRequest();
		}
	}


	
}


void UMatchmakingWidget::OnInitiateMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	//UE_LOG(LogTemp, Warning, TEXT("Response from initiate matchmaking %s"), *(Response->GetContentAsString()));

	if (bWasSuccessful) {
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject;

		//Create a reader pointer to read the json data
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		//Deserialize the json data given Reader and the actual object to deserialize
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			UE_LOG(LogAWS, Log, TEXT("response: %s"), *Response->GetContentAsString());

			MatchmakingTicketId = JsonObject->GetStringField("ticketId");

			UGameInstance* GameInstance = GetGameInstance();
			if (GameInstance != nullptr) {
				UGameLiftTutorialGameInstance* GameLiftTutorialGameInstance = Cast<UGameLiftTutorialGameInstance>(GameInstance);
				UE_LOG(LogAWS, Log, TEXT("Assigning matchmaking ticket %s"), *(MatchmakingTicketId));
				GameLiftTutorialGameInstance->MatchmakingTicketId = MatchmakingTicketId;
			}

			//AWS recommends to only poll every 10 seconds for optimization
			GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &UMatchmakingWidget::PollMatchmaking, 1.0f, false, 10.0f);
			SearchingForGame = true;


			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Initializing matchmaking request");


			//TODO - Handle currently looking for a match with widget stuff
		}
	}
}


void UMatchmakingWidget::PollMatchmaking()
{
	GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle);

	//Set current token status
	FetchCurrentTokenStatus();

	if (MatchmakingTicketId.Len() > 0.f) {

		// poll for matchmaking status
		TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
		RequestObj->SetStringField("ticketId", MatchmakingTicketId);

		FString RequestBody;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

		if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer) && AccessToken.Len() > 0) {
			//Initiate Matchmaking request
			TSharedRef<IHttpRequest> PollMatchmakingStatus = HttpModule->CreateRequest();
			PollMatchmakingStatus->OnProcessRequestComplete().BindUObject(this, &UMatchmakingWidget::OnPollMatchmakingResponseReceived);
			PollMatchmakingStatus->SetURL(PollMatchmakingUrl);
			PollMatchmakingStatus->SetVerb("GET");
			PollMatchmakingStatus->SetHeader("Authorization", AccessToken);
			PollMatchmakingStatus->ProcessRequest();
		}
		else {
			//Error with fetching the object, restart the polling
			GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &UMatchmakingWidget::PollMatchmaking, 1.0f, false, 10.0f);
		}
	}
	else
	{
		//Continuously recurse call until we have found the match
		GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &UMatchmakingWidget::PollMatchmaking, 1.0f, false, 10.0f);
	}
}

void UMatchmakingWidget::OnPollMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	//UE_LOG(LogTemp, Warning, TEXT("Response from initiate matchmaking %s"), *(Response->GetContentAsString()));

	if (bWasSuccessful) {
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject;

		//Create a reader pointer to read the json data
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		//Deserialize the json data given Reader and the actual object to deserialize
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			TSharedPtr<FJsonObject> Ticket = JsonObject->GetObjectField("ticket");
			FString TicketStatus = Ticket->GetObjectField("Type")->GetStringField("S");

			if (TicketStatus.Compare("MatchmakingSearching") == 0)
			{
				// continue to poll matchmaking
				GetWorld()->GetTimerManager().SetTimer(PollMatchmakingHandle, this, &UMatchmakingWidget::PollMatchmaking, 1.0f, false, 10.0f);
			}
			else if (TicketStatus.Compare("MatchmakingSucceeded") == 0) {
				// if check is to deal with a race condition involving the user pressing the cancel button
				if (SearchingForGame) {
					//MatchmakingButton->SetIsEnabled(false);
					SearchingForGame = false;

					//TODO - Reset the widget and say we successfully found a match
					//MatchmakingEventTextBlock->SetText(FText::FromString("Successfully found a match. Now connecting to the server"));

					UGameInstance* GameInstance = GetGameInstance();
					if (GameInstance != nullptr) {
						UGameLiftTutorialGameInstance* GameLiftTutorialGameInstance = Cast<UGameLiftTutorialGameInstance>(GameInstance);
						if (GameLiftTutorialGameInstance != nullptr) {
							GameLiftTutorialGameInstance->MatchmakingTicketId = FString("");
						}
					}
					
					// get the game session and player session details and connect to the server
					TSharedPtr<FJsonObject> GameSessionInfo = Ticket->GetObjectField("GameSessionInfo")->GetObjectField("M");
					FString IpAddress = GameSessionInfo->GetObjectField("IpAddress")->GetStringField("S");
					FString Port = GameSessionInfo->GetObjectField("Port")->GetStringField("N");
					FString PlayerSessionId = Ticket->GetObjectField("PlayerSessionId")->GetStringField("S");
					FString PlayerId = Ticket->GetObjectField("PlayerId")->GetStringField("S");
					FString LevelName = IpAddress + FString(":") + Port;
					const FString& Options = FString("?") + FString("PlayerSessionId=") + PlayerSessionId + FString("?PlayerId=") + PlayerId;

					//TODO - Handle invalid level travel
					/*if (GEngine->OnTravelFailure().IsBoundToObject(this) == false)
					{
						TravelFailureDelegateHandle = GEngine->OnTravelFailure().AddUObject(this, &UMenuWidget::OnTravelFailure);
					}*/

					UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false, Options);
				}
			}
			else if (TicketStatus.Compare("MatchmakingTimedOut") == 0 || TicketStatus.Compare("MatchmakingCancelled") == 0 || TicketStatus.Compare("MatchmakingFailed") == 0) {
				SearchingForGame = false;
				// TODO - Set the status of the search in the matchmaking widget
				/*UTextBlock* ButtonText = (UTextBlock*)MatchmakingButton->GetChildAt(0);
				ButtonText->SetText(FText::FromString("Join Game"));
				if (TicketStatus.Compare("MatchmakingCancelled") == 0) {
					MatchmakingEventTextBlock->SetText(FText::FromString("Matchmaking request was cancelled. Please try again"));
				}
				else if (TicketStatus.Compare("MatchmakingTimedOut") == 0) {
					MatchmakingEventTextBlock->SetText(FText::FromString("Matchmaking request timed out. Please try again"));
				}
				else if (TicketStatus.Compare("MatchmakingFailed") == 0) {
					MatchmakingEventTextBlock->SetText(FText::FromString("Matchmaking request failed. Please try again"));
				}*/
				// stop calling the PollMatchmaking function
				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance != nullptr) {
					UGameLiftTutorialGameInstance* GameLiftTutorialGameInstance = Cast<UGameLiftTutorialGameInstance>(GameInstance);
					if (GameLiftTutorialGameInstance != nullptr) {
						GameLiftTutorialGameInstance->MatchmakingTicketId = FString("");
					}
				}
			}
		} 
	}
}


void UMatchmakingWidget::CancelMatchmaking()
{
	if (SearchingForGame) {
		GetWorld()->GetTimerManager().ClearTimer(PollMatchmakingHandle); // stop searching for a match
		SearchingForGame = false;
		UE_LOG(LogTemp, Warning, TEXT("Cancel matchmaking"));

		if (MatchmakingTicketId.Len() > 0) {
			// cancel matchmaking request
			TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
			RequestObj->SetStringField("ticketId", MatchmakingTicketId);

			FString RequestBody;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);

			if (FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer) && AccessToken.Len() > 0) {
				// send a get request to google discovery document to retrieve endpoints
				TSharedRef<IHttpRequest> CancelMatchLookupRequest = HttpModule->CreateRequest();
				CancelMatchLookupRequest->OnProcessRequestComplete().BindUObject(this, &UMatchmakingWidget::OnCancelMatchmakingResponseReceived);
				CancelMatchLookupRequest->SetURL(CancelMatchLookupUrl);
				CancelMatchLookupRequest->SetVerb("POST");
				CancelMatchLookupRequest->SetHeader("Content-Type", "application/json");
				CancelMatchLookupRequest->SetHeader("Authorization", AccessToken);
				CancelMatchLookupRequest->SetContentAsString(RequestBody);
				CancelMatchLookupRequest->ProcessRequest();
			}
			else {
				/*UTextBlock* ButtonText = (UTextBlock*)MatchmakingButton->GetChildAt(0);
				ButtonText->SetText(FText::FromString("Join Game"));
				MatchmakingEventTextBlock->SetText(FText::FromString(""));

				MatchmakingButton->SetIsEnabled(true);*/
			}
		}
	}
}

void UMatchmakingWidget::OnCancelMatchmakingResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	//UE_LOG(LogTemp, Warning, TEXT("Response from cancel matchmaking %s"), *(Response->GetContentAsString()));

	if (bWasSuccessful) {
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject;

		//Create a reader pointer to read the json data
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		//Deserialize the json data given Reader and the actual object to deserialize
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			bool CancellationSuccessful = JsonObject->GetBoolField("success");
			if (CancellationSuccessful) {
				UGameInstance* GameInstance = GetGameInstance();
				if (GameInstance != nullptr) {
					UGameLiftTutorialGameInstance* GameLiftTutorialGameInstance = Cast<UGameLiftTutorialGameInstance>(GameInstance);
					if (GameLiftTutorialGameInstance != nullptr) {
						GameLiftTutorialGameInstance->IdToken = FString("");
					}
					UE_LOG(LogAWS, Log, TEXT("%s: cancellation successful"), __FUNCTIONW__);
				}
			}
		}
	}


}


void UMatchmakingWidget::FetchCurrentTokenStatus()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance != nullptr) {
		UGameLiftTutorialGameInstance* GameLiftTutorialGameInstance = Cast<UGameLiftTutorialGameInstance>(GameInstance);
		if (GameLiftTutorialGameInstance != nullptr) {
			MatchmakingTicketId = GameLiftTutorialGameInstance->MatchmakingTicketId;
			AccessToken = GameLiftTutorialGameInstance->AccessToken;
		}
	}
}

