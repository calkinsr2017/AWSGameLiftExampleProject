# AWSGameLiftExampleProject

This is an example project for building an unreal FPS with AWS gamelift


# How to build for server

In VS 2019, make sure you have the latest .NET Framwork SDK (4.8) and all previous targeting packs
Make sure you also have cmake downloaded and added to your environment paths. In command prombt, make sure "cmake --help" works

Environment variables Path
	- C:\Program Files\CMake\bin
	- C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin

New Variable 
	Name: VS150COMNTOOLS
	Value: C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools

steps to generate aws-cpp-sdk-gamelift-server.dll and .lib files:
	1) Download Amazon Gamelift Server SDK (4.0.0) 
		-Unzip and rename to ServerSDK so it has a shorter path name
		-We only interested in GameLift-Cpp-ServerSDK-3.4.0 and GameLift-Unreal-plugin-3.3.1 copy those to convient location with short file path
      		 (ServerSDK -> GameLift_04_06_2020 -> GameLift-SDK-Release-4.0.0)
	2) Run command prompt in administration mode.  cd into GameLift-Cpp-ServerSDK file. run the next sequence of commands
		-mkdir out
		-cd out
		-cmake -G "Visual Studio 16 2019" -A x64 -DBUILD_FOR_UNREAL=1 ..
		-msbuild ALL_BUILD.vcxproj /p:Configuration=Release

steps to build project for server
	1) put .dll and .lib generated files into (C:\Users\MPLEX\Desktop\GameLift-Unreal-plugin-3.3.1\UE4.24.3\GameLiftServerSDK\ThirdParty\GameLiftServerSDK\Win64)
	2) In project Plugins folder (Create one if it doesnt exist) and copy GameLiftServerSDK into it. (C:\Users\MPLEX\Desktop\GameLift-Unreal-plugin-3.3.1\UE4.24.3)
	3) Make a copy of ProjectNameEditor.Target.cs and rename to ProjectNameServer.Target.cs, in source folder. Change all the "Editor" to "Server" in code
		-add [SupportedPlatforms(UnrealPlatformClass.Server)] above plublic class definition
	4) in uproject file under "Plugins" section add "Name: "GameLiftServerSDK" and "Enabled": true
	5) in .Build.cs file add "GameLiftServerSDK" to PublicDependencyModuleNames 
	6) Regenerate VS project files
	7) build under Development_Editor win64
		-GameLiftServerSDK should now be in Unreal plugins
	8) add basic gamelift server code, and build uunder Development_Server win64

# Package project


# How to build for client


# Upload to AWS (Fleet, Queue)

# Matchmaking 