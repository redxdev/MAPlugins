// Copyright (c) MissiveArts LLC


#include "MAStateTreeCompareActorDistanceCondition.h"

#include "MAGameplay/AI/MAAITypeUtilities.h"
#include "MAStateTree/MAStateTree.h"
#include "VisualLogger/VisualLogger.h"

FStateTreeCompareActorDistanceCondition::FStateTreeCompareActorDistanceCondition()
{
	bInvert = false;
}

bool FStateTreeCompareActorDistanceCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!IsValid(InstanceData.Source) || !IsValid(InstanceData.Target))
	{
		UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: invalid source or target"), *Name.ToString());
		return false;
	}

	const FVector::FReal Left = FVector::DistSquared(InstanceData.Source->GetActorLocation(), InstanceData.Target->GetActorLocation());
	const FVector::FReal Right = FMath::Square(InstanceData.Distance);

	bool bResult = MAGameplay::AI::Compare(Left, Right, Operator);
	if (bInvert)
	{
		bResult = !bResult;
	}

	return bResult;
}
