// Copyright (c) MissiveArts LLC


#include "MASmartObjectLibrary.h"
#include "BlackboardKeyType_SOClaimHandle.h"
#include "BehaviorTree/BlackboardComponent.h"

FSmartObjectClaimHandle UMASmartObjectLibrary::GetValueAsSOClaimHandle(UBlackboardComponent* BlackboardComponent, const FBlackboardKeySelector& KeySelector)
{
	if (!BlackboardComponent)
	{
		return FSmartObjectClaimHandle::InvalidHandle;
	}

	return BlackboardComponent->GetValue<UBlackboardKeyType_SOClaimHandle>(KeySelector.GetSelectedKeyID());
}
