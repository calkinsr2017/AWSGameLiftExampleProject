// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMELIFTTUTORIAL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	//The "BeginPlay" but for widget classes
	void NativeConstruct() override;
};
