// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GameLiftTutorialGameMode.h"
#include "GameLiftTutorialCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGameLiftTutorialGameMode::AGameLiftTutorialGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
		//Test
	}
}

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

		if (!StartTCPReceiver("SocketListener", ))

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