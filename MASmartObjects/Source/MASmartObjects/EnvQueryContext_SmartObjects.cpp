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

	TArray<FSmartObjectRequestResult> Results;
	for (const FVector& Location : ContextLocations)
	{
		// Not technically a radius, but... close enough.
		Request.QueryBox = FBox(Location - SearchRadius, Location + SearchRadius);
		SO->FindSmartObjects(Request, Results);
	}

	UEnvQueryItemType_SmartObject::SetContextHelper(ContextData, SO, Results);
}
