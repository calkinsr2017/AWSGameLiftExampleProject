// Fill out your copyright notice in the Description page of Project Settings.


#include "TextReaderComponent.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

FString UTextReaderComponent::ReadFile(FString filePath)
{
	//Read file from [project]/Content/
	FString directory = FPaths::ProjectContentDir();
	FString result;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	if (file.CreateDirectory(*directory)) {
		FString myFile = directory + "/" + filePath;
		FFileHelper::LoadFileToString(result, *myFile);
	}
	return result;
}
