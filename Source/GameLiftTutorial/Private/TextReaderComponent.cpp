// Fill out your copyright notice in the Description page of Project Settings.


#include "TextReaderComponent.h"


#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"

// Sets default values for this component's properties
UTextReaderComponent::UTextReaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

FString UTextReaderComponent::ReadFile(FString filePath)
{
	//Read file from [project]/filePath/
	FString FolderPath = FPaths::ProjectContentDir() + "Credentials";
	FString Directory = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FolderPath);

	FString Result = "";
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	if (file.CreateDirectory(*Directory)) {
		FString myFile = Directory + "/" + filePath;

		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, "Attempting to locate: " + myFile);
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, "in directory: " + Directory);
		}

		FFileHelper::LoadFileToString(Result, *myFile);

		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, "Result: " + Result);
		}
	}

	return Result;
}
