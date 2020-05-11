// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "TextReaderComponent.h"

// Add default functionality here for any IMainMenuWidget functions that are not pure virtual.
UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	TextReader = CreateDefaultSubobject<UTextReaderComponent>(TEXT("TextReaderComp"));

	AccessKey = TextReader->ReadFile("Credentials/AWS_AccessKey.txt");
	SecretKey = TextReader->ReadFile("Credentials/AWS_SecretKey.txt");
	QueueName = TextReader->ReadFile("Credentials/AWS_QueueName.txt");
	
	//AttemptToJoinGameFinished = true; // default value true so that we can prevent double clicks
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
	//Client = UGameLiftClientObject::CreateGameLiftObject(AccessKey, SecretKey, "us-east-2", true, 9081);
	//Client = UGameLiftClientObject::CreateGameLiftObject(AccessKey, SecretKey, "us-east-2");

}