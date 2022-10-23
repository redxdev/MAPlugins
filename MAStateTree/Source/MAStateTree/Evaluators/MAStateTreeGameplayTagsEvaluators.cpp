// Copyright (c) MissiveArts LLC

#include "MAStateTreeGameplayTagsEvaluators.h"
#include "GameplayTagAssetInterface.h"
#include "StateTreeExecutionContext.h"

void FMAStateTreeGameplayTagInterfaceEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);

	bool bRetrievedTags = false;
	if (IsValid(InstanceData.Target))
	{
		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(InstanceData.Target))
		{
			bRetrievedTags = true;
			TagInterface->GetOwnedGameplayTags(InstanceData.Tags);
		}
	}

	if (!bRetrievedTags)
	{
		InstanceData.Tags.Reset();
	}
}

void FMAStateTreeGameplayTagInterfaceEvaluator::TreeStop(FStateTreeExecutionContext& Context) const
{
	FInstanceData& InstanceData = Context.GetInstanceData<FInstanceData>(*this);
	InstanceData.Tags.Reset(0);
}
