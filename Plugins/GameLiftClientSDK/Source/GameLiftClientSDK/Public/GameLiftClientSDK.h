// Created by YetiTech Studios.

#pragma once

#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

class FGameLiftClientSDKModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	
	static void* GameLiftClientSDKLibraryHandle;
	static bool LoadDependency(const FString& Dir, const FString& Name, void*& Handle);
	static void FreeDependency(void*& Handle);
};
