// Copyright (c) MissiveArts LLC


#include "EnvQueryGenerator_SmartObjects.h"
#include "EnvQueryItemType_SmartObject.h"
#include "GameplayTagAssetInterface.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"

#define LOCTEXT_NAMESPACE "MASmartObjects_EnvQueryGenerator"

UEnvQueryGenerator_SmartObjects::UEnvQueryGenerator_SmartObjects()
{
	ItemType = UEnvQueryItemType_SmartObject::StaticClass();
	SearchRadius.DefaultValue = 1000.f;
	SearchCenter = UEnvQueryContext_Querier::StaticClass();
}

void UEnvQueryGenerator_SmartObjects::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	if (!SearchCenter)
	{
		return;
	}

	USmartObjectSubsystem* Subsystem = USmartObjectSubsystem::GetCurrent(QueryInstance.World);
	if (!Subsystem)
	{
		return;
	}

	FGameplayTagContainer UserTags;
	if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(QueryInstance.Owner.Get()))
	{
		TagInterface->GetOwnedGameplayTags(UserTags);
	}

	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(SearchCenter, ContextLocations))
	{
		return;
	}

	SearchRadius.BindData(QueryInstance.Owner.Get(), QueryInstance.QueryID);
	float Radius = SearchRadius.GetValue();

	FSmartObjectRequest Request;
	Request.Filter.UserTags = UserTags;
	Request.Filter.ActivityRequirements = ActivityRequirements;

	TArray<FSmartObjectRequestResult> Results;
	for (const FVector& Location : ContextLocations)
	{
		// Not technically a radius, but... close enough.
		Request.QueryBox = FBox(Location - Radius, Location + Radius);

		Subsystem->FindSmartObjects(Request, Results);
	}

	QueryInstance.AddItemData<UEnvQueryItemType_SmartObject>(Results);
}

FText UEnvQueryGenerator_SmartObjects::GetDescriptionTitle() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DescriptionTitle"), Super::GetDescriptionTitle());

	return FText::Format(LOCTEXT("Title", "{DescriptionTitle}: generate set of smart object slots in radius"), Args);
}

FText UEnvQueryGenerator_SmartObjects::GetDescriptionDetails() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("Radius"), FText::FromString(SearchRadius.ToString()));
	Args.Add(TEXT("ActivityRequirements"), FText::FromString(ActivityRequirements.GetDescription()));

	return FText::Format(LOCTEXT("DescriptionDetails", "radius: {Radius}\nactivity requirements: {ActivityRequirements}"), Args);
}
