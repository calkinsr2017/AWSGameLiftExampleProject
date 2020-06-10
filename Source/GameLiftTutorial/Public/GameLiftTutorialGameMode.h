#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "HttpModule.h"
#include "GameFramework/GameModeBase.h"
#include "GameLiftTutorialGameMode.generated.h"

UENUM()
enum class EUpdateReason : uint8 {
	MATCHMAKING_DATA_UPDATED = 0,
	BACKFILL_FAILED,
	BACKFILL_TIMED_OUT,
	BACKFILL_CANCELLED
};

USTRUCT()
struct FStartGameSessionState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	FStartGameSessionState() {
		Status = false;
	}
};

/* Will enable us to get all players in the match */
USTRUCT()
struct FUpdateGameSessionState
{
	GENERATED_BODY();

	FUpdateGameSessionState() {

	}

	//What is the current state in the game session, actively updated on server aws tick
	UPROPERTY()
		EUpdateReason Reason;
	//Will be a running list of all players actively looking for matchmaking
		TMap<FString, FPlayer> PlayerIdToPlayer;
};

USTRUCT()
struct FProcessTerminateState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	long TerminationTime;

	FProcessTerminateState() {
		Status = false;
	}
};

USTRUCT()
struct FHealthCheckState
{
	GENERATED_BODY();

	UPROPERTY()
		bool Status;

	FHealthCheckState() {
		Status = false;
	}
};

UCLASS(minimalapi)
class AGameLiftTutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameLiftTutorialGameMode();

	//Called when the player logs into the map
	void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	              FString& ErrorMessage) override;

	//Called when player leaves the map
	void Logout(AController* ExitingPC) override;
	FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	                      const FString& Options,
	                      const FString& Portal) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
		FStartGameSessionState StartGameSessionState;

	UPROPERTY()
		FUpdateGameSessionState UpdateGameSessionState;

	UPROPERTY()
		FProcessTerminateState ProcessTerminateState;

	UPROPERTY()
		FHealthCheckState HealthCheckState;

	
	FHttpModule* HttpModule;
};