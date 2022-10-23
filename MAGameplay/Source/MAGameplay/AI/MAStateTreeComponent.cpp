// Copyright (c) MissiveArts LLC

#include "MAStateTreeComponent.h"

UMAStateTreeComponent::UMAStateTreeComponent()
{
	bStartLogicAutomatically = false;
}

void UMAStateTreeComponent::SetStateTreeRef(const FStateTreeReference& InStateTreeRef)
{
	if (bIsRunning)
	{
		StopLogic(TEXT("Updating state tree ref"));
	}

	StateTreeRef = InStateTreeRef;
}
