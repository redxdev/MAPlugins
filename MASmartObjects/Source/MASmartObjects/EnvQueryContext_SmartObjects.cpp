// Copyright (c) MissiveArts LLC. All rights reserved.


#include "EnvQueryContext_SmartObjects.h"
#include "EnvQueryItemType_SmartObject.h"
#include "GameplayTagAssetInterface.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"

UEnvQueryContext_SmartObjects::UEnvQueryContext_SmartObjects()
{
	SearchCenter = UEnvQueryContext_Querier::StaticClass();
}

static int32 GetTypeHash(const FSmartObjectRequestResult& Result)
{
	return HashCombine(GetTypeHash(Result.SmartObjectHandle), GetTypeHash(Result.SlotHandle));
}

void UEnvQueryContext_SmartObjects::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	if (!QueryInstance.World)
	{
		return;
	}

	if (!SearchCenter)
	{
		return;
	}

	FGameplayTagContainer UserTags;
	if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(QueryInstance.Owner.Get()))
	{
		TagInterface->GetOwnedGameplayTags(UserTags);
	}

	USmartObjectSubsystem* SO = QueryInstance.World->GetSubsystem<USmartObjectSubsystem>();
	FSmartObjectRequest Request;
	Request.Filter = FSmartObjectRequestFilter(UserTags, ActivityRequirements);

	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(SearchCenter, ContextLocations))
	{
		return;
	}

	TSet<FSmartObjectRequestResult> Results;
	for (const FVector& Location : ContextLocations)
	{
		// Not technically a radius, but... close enough.
		Request.QueryBox = FBox(Location - SearchRadius, Location + SearchRadius);

		TArray<FSmartObjectRequestResult> FoundObjects;
		if (SO->FindSmartObjects(Request, FoundObjects))
		{
			Results.Append(FoundObjects);
		}
	}

	UEnvQueryItemType_SmartObject::SetContextHelper(ContextData, SO, Results.Array());
}
