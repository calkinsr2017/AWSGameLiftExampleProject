// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TextReaderComponent.generated.h"

/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API UTextReaderComponent : public UObject
{
	GENERATED_BODY()

public:
	FString ReadFile(FString filePath);
};
