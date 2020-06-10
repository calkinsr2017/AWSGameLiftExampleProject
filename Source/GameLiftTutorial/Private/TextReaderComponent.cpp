// Fill out your copyright notice in the Description page of Project Settings.


#include "TextReaderComponent.h"



#include "Engine/Engine.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"


UTextReaderComponent::UTextReaderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

FString UTextReaderComponent::ReadFile(FString filePath)
{
	//Read file from [project]/filePath/
	FString DirectoryPath = FPaths::ProjectContentDir();
	FString FullPath = DirectoryPath + "/" + filePath;
	FString Result;
	IPlatformFile& file = FPlatformFileManager::Get().GetPlatformFile();
	//IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*DirectoryPath);

	if(file.FileExists(*FullPath))
	{
		FFileHelper::LoadFileToString(Result, *FullPath);
	}
	


	return Result;
}
