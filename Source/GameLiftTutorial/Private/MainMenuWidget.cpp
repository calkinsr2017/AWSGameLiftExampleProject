// Fill out your copyright notice in the Description page of Project Settings.

//#include "GameLiftTutorial.h"
#include "MainMenuWidget.h"
#include "TextReaderComponent.h"
#include "GameLiftClientObject.h"
#include "GameLiftClientApi.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Add default functionality here for any IMainMenuWidget functions that are not pure virtual.
UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	AccessKey = TextReader->ReadFile("Credentials/AWS_AccessKey.txt");
	SecretKey = TextReader->ReadFile("Credentials/AWS_SecretKey.txt");
	QueueName = TextReader->ReadFile("Credentials/AWS_QueueName.txt");
	
	AttemptToJoinGameFinished = true; // default value true so that we can prevent double clicks
	FailedToJoinGame = false;
	SucceededToJoinGame = false;
	//GameSessionsLeft = 4;

	//DescribeGameSessionQueuesEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	//SearchGameSessionsEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);

	//CreatePlayerSessionEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	//StartGameSessionPlacementEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
	//DescribeGameSessionPlacementEvent = FGenericPlatformProcess::GetSynchEventFromPool(false);
}


void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// line below is for testing local gamelift
#if WITH_GAMELIFTCLIENTSDK
	Client = UGameLiftClientObject::CreateGameLiftObject(AccessKey, SecretKey, "us-east-2", true, 9081);
	//Client = UGameLiftClientObject::CreateGameLiftObject(AccessKey, SecretKey, "us-east-2");
#endif
	
}

void UMainMenuWidget::CreatePlayerSession(const FString& GameSessionId, const FString& PlayerSessionId) {
	UGameLiftCreatePlayerSession* CreatePlayerSessionObject = Client->CreatePlayerSession(GameSessionId, PlayerSessionId);
	CreatePlayerSessionObject->OnCreatePlayerSessionSuccess.AddDynamic(this, &UMainMenuWidget::OnCreatePlayerSessionSuccess);
	CreatePlayerSessionObject->OnCreatePlayerSessionFailed.AddDynamic(this, &UMainMenuWidget::OnCreatePlayerSessionFailed);
	CreatePlayerSessionObject->Activate();
}

void UMainMenuWidget::JoinGame()
{
	AttemptToJoinGameFinished = false;

	FailedToJoinGame = false;
	SucceededToJoinGame = false;

	//TODO - Describe queue

	if (AttemptToJoinGameFinished)
	{
		if (SucceededToJoinGame) {
			// don't reenable anything since game was successfully joined
		}
		else if (FailedToJoinGame) {
			//JoinGameButton->SetIsEnabled(true);
			//EnableMouseEvents();
		}
		return;
	}

	//Queues to do here
	const FString& PlacementId = GenerateRandomId();
	StartGameSessionPlacement(QueueName, 100, PlacementId);
	//StartGameSessionPlacementEvent->Wait();

	AttemptToJoinGameFinished = true;

}

void UMainMenuWidget::StartGameSessionPlacement(const FString& QueueNameInput, const int& MaxPlayerCount, const FString& PlacementId) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("start game session placement called in main menu widget"));
	//UGameLiftStartGameSessionPlacement* StartGameSessionPlacementObject = Client->StartGameSessionPlacement(QueueNameInput, MaxPlayerCount, PlacementId);
	//StartGameSessionPlacementObject->OnStartGameSessionPlacementSuccess.AddDynamic(this, &UMainMenuWidget::OnStartGameSessionPlacementSuccess);
	//StartGameSessionPlacementObject->OnStartGameSessionPlacementFailed.AddDynamic(this, &UMainMenuWidget::OnStartGameSessionPlacementFailed);
	//StartGameSessionPlacementObject->Activate();
}

void UMainMenuWidget::OnCreatePlayerSessionSuccess(const FString& IPAddress, const FString& Port, const FString& PlayerSessionID) {

	//UE_LOG(LogAWS, Log, TEXT("Player Session Creation Succeeded."));
	//if (RequestConnection(PlayerSessionID, IPAddress)) {
		FString LevelName = IPAddress + FString(":") + Port;
		const FString& Options = FString("?") + FString("PlayerSessionId=") + PlayerSessionID;

		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName), false, Options);

		AttemptToJoinGameFinished = true;
		SucceededToJoinGame = true;
		FailedToJoinGame = false;
	//}

	/*else if (PlayerSessionStatus == 2) {
		// already activated?
	}
	else {
		// pending or timed out?
	}
	CreatePlayerSessionEvent->Trigger();*/
}

void UMainMenuWidget::OnCreatePlayerSessionFailed(const FString& ErrorMessage) {
	//UE_LOG(LogAWS, Log, TEXT("on create player session failed %s"), *ErrorMessage);
	//CreatePlayerSessionEvent->Trigger();
}

FString UMainMenuWidget::GenerateRandomId() {
	int RandOne = FMath::RandRange(0, 200000);
	int RandTwo = FMath::RandRange(0, 200000);
	int RandThree = FMath::RandRange(0, 200000);

	FTimespan Timespan = UKismetMathLibrary::GetTimeOfDay(FDateTime::Now());
	double MilliSeconds = Timespan.GetTotalMilliseconds();

	FString RandOneString = FString::FromInt(RandOne);
	FString RandTwoString = FString::FromInt(RandTwo);
	FString RandThreeString = FString::FromInt(RandThree);
	FString MilliSecondsString = FString::SanitizeFloat(MilliSeconds);
	//UE_LOG(LogMyMainMenu, Log, TEXT("RandOne %s RandTwo %s RandTree %s Current milliseconds %s"),*RandOneString, *RandTwoString, *RandThreeString, *MilliSecondsString);

	RandOne = MilliSeconds - RandOne;
	RandTwo = MilliSeconds - RandTwo;
	RandThree = MilliSeconds - RandThree;
	FString Id = FString::FromInt(RandOne) + FString("-") + FString::FromInt(RandTwo) + FString("-") + FString::FromInt(RandThree);

	return Id;
}


/*bool UMainMenuWidget::RequestConnection(const FString& PlayerSessionID, const FString& IPAddress)
{
	FSocket* Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	//addr->SetIp(IPAddress);
	bool isValid;
	addr->SetIp(*IPAddress, isValid);
	//FString testIp = "127.0.0.1";
	//addr->SetIp(*testIp, isValid);
	if (!isValid)
	{
		UE_LOG(CDLogOnline, Warning, TEXT("Invalid IP given."));
	}
	//addr->SetPort(FCString::Atoi(*Port));
	addr->SetPort(7776);

	bool connected = Socket->Connect(*addr);

	if (connected)
	{
		FString serialized = PlayerSessionID;
		TCHAR* serializedChar = serialized.GetCharArray().GetData();
		int32 size = FCString::Strlen(serializedChar);
		int32 sent = 0;

		bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
		UE_LOG(CDLogOnline, Warning, TEXT("Sent request to server."));
		if (!successful)
		{
			UE_LOG(CDLogOnline, Warning, TEXT("Could not send player session id to game server."));
		}

		// Wait for 5 seconds, and then see if there are any pending messages.
		FPlatformProcess::Sleep(15.0f);

		TArray<uint8> ReceivedData;
		uint32 Size;
		while (Socket->HasPendingData(Size))
		{
			ReceivedData.Init(0, FMath::Min(Size, 65507u));

			int32 Read = 0;
			Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
		}

		if (ReceivedData.Num() <= 0)
		{
			// No Data Received
			UE_LOG(CDLogOnline, Warning, TEXT("No Response received from the server."));
		}

		const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
		UE_LOG(CDLogOnline, Warning, TEXT("Received message from server: %s"), *ReceivedUE4String);
		//return ReceivedUE4String.Contains("Success");
		return true;
	}
	else
	{
		UE_LOG(CDLogOnline, Warning, TEXT("Connecting socket failed."));
	}

	return false;
}*/
