// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameLiftTutorial : ModuleRules
{
	public GameLiftTutorial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "GameLiftServerSDK", "UMG", "SlateCore", "Http", "Json", "JsonUtilities", "WebBrowserWidget",
			"LibOVRPlatform", //for access to C SDK functionality in code
            "OnlineSubsystem"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "OnlineSubsystem", //Access to Interfaces like Friends, Sessions, etc.
            "OnlineSubsystemOculus", //Access to Oculus specific code
        });
	}
}
