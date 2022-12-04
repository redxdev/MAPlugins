// Copyright (c) MissiveArts LLC

#include "MAStateTreeWeightedRandomTagTask.h"
#include "StateTreeExecutionContext.h"
#include "MAStateTree/MAStateTree.h"
#include "VisualLogger/VisualLogger.h"

EStateTreeRunStatus FMAStateTreeWeightedRandomTagTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	InstanceData.ChosenTag = FGameplayTag::EmptyTag;

	if (InstanceData.TagWeights.IsEmpty())
	{
		UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: no tag weights specified, failing"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	float TotalWeight = 0.f;
	for (const auto& KV : InstanceData.TagWeights)
	{
		TotalWeight += KV.Value;
	}

	if (TotalWeight <= 0.f)
	{
		UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: tag weights add up to <= 0, failing"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	for (const auto& KV : InstanceData.TagWeights)
	{
		if (KV.Value > 0)
		{
			RandomValue -= KV.Value;
			if (RandomValue <= 0.f)
			{
				InstanceData.ChosenTag = KV.Key;
				break;
			}
		}
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FMAStateTreeWeightedRandomTagTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	return EStateTreeRunStatus::Running;
}
