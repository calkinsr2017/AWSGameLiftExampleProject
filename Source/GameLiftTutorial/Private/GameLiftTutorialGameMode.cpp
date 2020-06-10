// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GameLiftTutorialGameMode.h"

#include "GameLiftTutorial.h"
#include "GameLiftTutorialCharacter.h"
#include "HttpModule.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AGameLiftTutorialGameMode::AGameLiftTutorialGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HttpModule = &FHttpModule::Get();
}

/* ================================ GAME MODE SPECIFIC FUNCTIONS THAT CAN BE USED FOR OVERRIDE ========================================= */
void AGameLiftTutorialGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) {
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	UE_LOG(LogAWS, Log, TEXT("Player with Unreal Id of: %s is logging into the map."), *UniqueId.ToString());

	// PreLogin mode should add a player session to the mix

	// kick the player out id the player did not pass a valid player session id
#if WITH_GAMELIFT

	if (*Options && Options.Len() > 0) {
		//From the GameInstance, we get this token option
		const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
		if (PlayerSessionId.Len() > 0.f) {
			auto AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
			if (!AcceptPlayerSessionOutcome.IsSuccess())
			{
				ErrorMessage = FString("Unauthorized");
			}

			//Set request parameters
			Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
			DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

			// Call DescribePlayerSessions
			auto DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
			if (DescribePlayerSessionsOutcome.IsSuccess()) {
				UE_LOG(LogAWS, Log, TEXT("Describe player sessions call succeeded!"));
				auto DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
				uint8 Count = 1;
				//Do a call to get our own PlayerId just to debug what that looks like
				auto PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
				if (PlayerSessions != nullptr) {
					auto PlayerSession = PlayerSessions[0];
					FString PlayerId = PlayerSession.GetPlayerId();

					UE_LOG(LogAWS, Log, TEXT("Player id associated with player session id %s is %s"), *PlayerSessionId, *PlayerId);
				}
				else {
					UE_LOG(LogAWS, Error, TEXT("Describe player sessions call failed :("));
				}
			}
		}
		else {
			ErrorMessage = FString("Unauthorized");
		}
	}
	else {
		ErrorMessage = FString("Unauthorized");
	}
#endif 
}

void AGameLiftTutorialGameMode::Logout(AController* ExitingPC) {
	Super::Logout(ExitingPC);

	//Make sure to remove the player session on the dedicated server after it is done
	APlayerState* State = ExitingPC->PlayerState;
	if (State != nullptr) {
		//AGameLiftTutorialPlayerState* PlayerState = Cast<AGameLiftTutorialPlayerState>(State);

		//TODO - Once custom player state is setup with this player session id, logout correctly here
		/*const FString& PlayerSessionId = State->PlayerSessionId;
		if (PlayerSessionId.Len() > 0) {
#if WITH_GAMELIFT
			Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
#endif
		}*/
	}
}

FString AGameLiftTutorialGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) {
	FString InitializedString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	// UNCOMMENT BELOW FOR TESTING LOCALLY
	/*APlayerState* State = NewPlayerController->PlayerState;
	if (State != nullptr) {
		AGameLiftTutorialPlayerState* PlayerState = Cast<AGameLiftTutorialPlayerState>(State);
		PlayerState->Team = "cowboys";
	}*/
	UE_LOG(LogTemp, Warning, TEXT("inside init new player"));

	if (*Options && Options.Len() > 0) {
		const FString& PlayerSessionId = UGameplayStatics::ParseOption(Options, "PlayerSessionId");
#if WITH_GAMELIFT
		// Set request parameters
		Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
		DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

		// Call DescribePlayerSessions
		auto DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
		if (DescribePlayerSessionsOutcome.IsSuccess()) {
			UE_LOG(LogTemp, Warning, TEXT("describe player sessions succeeded"));
			auto DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
			int Count = 1;
			auto PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
			if (PlayerSessions != nullptr) {
				auto PlayerSession = PlayerSessions[0];
				FString PlayerId = PlayerSession.GetPlayerId();
				UE_LOG(LogTemp, Warning, TEXT("player id associated with player session id %s is %s"), *PlayerSessionId, *PlayerId);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("player sessions null"));
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("describe player sessions failed"));
		}
#endif
		const FString& PlayerId = UGameplayStatics::ParseOption(Options, "PlayerId");
		UE_LOG(LogTemp, Warning, TEXT("Player session id in init new player: %s"), *(PlayerSessionId));

		APlayerState* State = NewPlayerController->PlayerState;
		if (State != nullptr) {
			//TODO - We need the player state information to be set up
			/*AGameLiftTutorialPlayerState* PlayerState = Cast<AGameLiftTutorialPlayerState>(State);
			if (PlayerState != nullptr) {
				PlayerState->PlayerSessionId = *PlayerSessionId;
				PlayerState->PlayerId = *PlayerId;
				UE_LOG(LogTemp, Warning, TEXT("state is not null in init new player"));

				// assign player's mesh color based on the player's team
				if (UpdateGameSessionState.PlayerIdToPlayer.Num() > 0) {
					UE_LOG(LogTemp, Warning, TEXT("Updategamesessionstate is not null and PlayerIdToPlayer is populated"));
					if (UpdateGameSessionState.PlayerIdToPlayer.Contains(PlayerId)) {
						FPlayer* PlayerObj = UpdateGameSessionState.PlayerIdToPlayer.Find(PlayerId);
						FString Team = PlayerObj->m_team;
						PlayerState->Team = *Team;
					}
				}
				else if (StartGameSessionState.PlayerIdToPlayer.Num() > 0) {
					UE_LOG(LogTemp, Warning, TEXT("StartGameSessionState is not null and PlayerIdToPlayer is populated"));

					if (StartGameSessionState.PlayerIdToPlayer.Contains(PlayerId)) {
						FPlayer* PlayerObj = StartGameSessionState.PlayerIdToPlayer.Find(PlayerId);
						FString Team = PlayerObj->m_team;
						PlayerState->Team = *Team;
					}
				}
			}*/
		}
	}
	return InitializedString;
}

/* ============================================================================================================== */

void AGameLiftTutorialGameMode::BeginPlay() {
	Super::BeginPlay();

#if WITH_GAMELIFT
	auto InitSDKOutcome = Aws::GameLift::Server::InitSDK();

	//call back functions
	if (InitSDKOutcome.IsSuccess()) {

		//bridge between game session being invoked by the server SDK and our game. Has info on players involved
		auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession GameSessionObj, void* Params)
		{
			FStartGameSessionState* State = (FStartGameSessionState*)Params;

			State->Status = Aws::GameLift::Server::ActivateGameSession().IsSuccess();
		};

		//contains info on players that want to join as well as current players. Will be used for backfile
		auto OnUpdateGameSession = [](Aws::GameLift::Server::Model::UpdateGameSession UpdateGameSessionObj, void* Params)
		{
			FUpdateGameSessionState* State = (FUpdateGameSessionState*)Params;

			Aws::GameLift::Server::Model::UpdateReason Reason = UpdateGameSessionObj.GetUpdateReason();
			//This is where we will be listening for the flex matchmaker
			if (Reason == Aws::GameLift::Server::Model::UpdateReason::MATCHMAKING_DATA_UPDATED)
			{
				State->Reason = EUpdateReason::MATCHMAKING_DATA_UPDATED;
				//If we were able to have matchmaking be updated - extract the data from it
				Aws::GameLift::Server::Model::GameSession GameSessionObj = UpdateGameSessionObj.GetGameSession();

				//Get the JSON object returned from Flex Matchmaker as a string
				FString FlexMatchmakerData = GameSessionObj.GetMatchmakerData();

				// Create a pointer to hold the json serialized data
				TSharedPtr<FJsonObject> JsonObject;

				//Create a reader pointer to read the json data
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FlexMatchmakerData);

				//Deserialize the json data given Reader and the actual object to deserialize
				if (FJsonSerializer::Deserialize(Reader, JsonObject))
				{
					TArray<TSharedPtr<FJsonValue>> Teams = JsonObject->GetArrayField("teams");
					for (TSharedPtr<FJsonValue> Team : Teams) {
						TSharedPtr<FJsonObject> TeamObj = Team->AsObject();
						FString TeamName = TeamObj->GetStringField("name");
						if (TeamName.Len() > 0) {
							UE_LOG(LogTemp, Warning, TEXT("team: %s"), *(TeamName));
						}
						TArray<TSharedPtr<FJsonValue>> Players = TeamObj->GetArrayField("players");

						for (TSharedPtr<FJsonValue> Player : Players) {
							TSharedPtr<FJsonObject> PlayerObj = Player->AsObject();
							FString PlayerId = PlayerObj->GetStringField("playerId");
							TSharedPtr<FJsonObject> Attributes = PlayerObj->GetObjectField("attributes");
							TSharedPtr<FJsonObject> Skill = Attributes->GetObjectField("skill");
							FString SkillValue = Skill->GetStringField("valueAttribute");
							FAttributeValue SkillAttributeValue;
							SkillAttributeValue.m_N = FCString::Atof(*SkillValue);

							FPlayer FPlayerObj;

							if (PlayerId.Len() > 0) {
								UE_LOG(LogAWS, Log, TEXT("Player id: %s"), *(PlayerId));
							}
							FPlayerObj.m_playerId = PlayerId;
							FPlayerObj.m_team = TeamName;
							FPlayerObj.m_playerAttributes.Add("skill", SkillAttributeValue);
							//TODO: FIGURE OUT HOW TO GET THE PING INFORMATION FOR BELOW
							//FPlayerObj.m_latencyInMs.Add(? , ? );
							State->PlayerIdToPlayer.Add(PlayerId, FPlayerObj);
						}
					}
				}
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_FAILED) {
				State->Reason = EUpdateReason::BACKFILL_FAILED;
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_TIMED_OUT) {
				State->Reason = EUpdateReason::BACKFILL_TIMED_OUT;
			}
			else if (Reason == Aws::GameLift::Server::Model::UpdateReason::BACKFILL_CANCELLED) {
				State->Reason = EUpdateReason::BACKFILL_CANCELLED;
			}
		};


		//called if one of 3 conditions is met. 1) Gamelift determined server processes is unhealthy 2) EC2 interuption occurs 3) fleet instance compacity goes down
		//Gamelift telling us that the process will end soon and we have responsibility do shut things down
		auto OnProcessTerminate = [](void* Params)
		{
			FProcessTerminateState* State = (FProcessTerminateState*)Params;
			//relays information back to the server
			auto GetTerminationTimeOutcome = Aws::GameLift::Server::GetTerminationTime();
			if (GetTerminationTimeOutcome.IsSuccess()) {
				State->TerminationTime = GetTerminationTimeOutcome.GetResult();
			}

			auto ProcessEndingOutcome = Aws::GameLift::Server::ProcessEnding();

			if (ProcessEndingOutcome.IsSuccess()) {
				State->Status = true;
				FGenericPlatformMisc::RequestExit(false);
			}
		};

		//called every 60 seconds by gamelift to check if server process is healthy. If for whatever reason server process is unhealthy, the above callback function will be invoked
		auto OnHealthCheck = [](void* Params)
		{
			FHealthCheckState* State = (FHealthCheckState*)Params;
			State->Status = true;

			return State->Status;
		};


		//get command line arguments
		TArray<FString> CommandLineTokens;
		TArray<FString> CommandLineSwitches;
		int Port = FURL::UrlConfig.DefaultPort;

		//GameLiftTutorialServer.exe token -port=7777 the -port is a switch, the token is a token
		FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);

		for (FString Str : CommandLineSwitches) {
			FString Key;
			FString Value;

			//retrieve port number from command line arguments
			if (Str.Split("=", &Key, &Value)) {
				if (Key.Equals("port")) {
					Port = FCString::Atoi(*Value);
				}
			}
		}

		//every time a gamesession starts gamelift generates files for loging, that file will be called aLogFile.txt
		const char* LogFile = "aLogFile.txt";
		const char** LogFiles = &LogFile;
		auto LogParams = new Aws::GameLift::Server::LogParameters(LogFiles, 1);

		//parameters to pass to processReady
		auto Params = new Aws::GameLift::Server::ProcessParameters(
			OnStartGameSession,
			&StartGameSessionState,
			OnUpdateGameSession,
			&UpdateGameSessionState,
			OnProcessTerminate,
			&ProcessTerminateState,
			OnHealthCheck,
			&HealthCheckState,
			Port,
			*LogParams
		);

		//notify gamelift that the server  process is now ready to host game sessions

		auto ProcessReadyOutcome = Aws::GameLift::Server::ProcessReady(*Params);


	}
#endif

}