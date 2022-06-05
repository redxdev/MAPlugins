// Copyright (c) MissiveArts LLC. All rights reserved.


#include "EnvQueryContext_Player.h"
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	if (!QueryInstance.World)
	{
		return;
	}

	TArray<AActor*> PlayerPawns;
	for (TActorIterator<APlayerController> It(QueryInstance.World); It; ++It)
	{
		APlayerController* PC = *It;
		if (PC && PC->GetPawn())
		{
			PlayerPawns.Add(PC->GetPawn());
		}
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerPawns);
}
