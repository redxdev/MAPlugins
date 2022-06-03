// Copyright (c) MissiveArts LLC. All rights reserved.

#include "BTComposite_Random.h"

UBTComposite_Random::UBTComposite_Random()
{
	NodeName = "Random";
}

int32 UBTComposite_Random::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;
	if (GetChildrenNum() > 0 && (PrevChild == BTSpecialChild::NotInitialized || LastResult == EBTNodeResult::Failed))
	{
		NextChildIdx = FMath::RandHelper(GetChildrenNum());
	}

	return NextChildIdx;
}
