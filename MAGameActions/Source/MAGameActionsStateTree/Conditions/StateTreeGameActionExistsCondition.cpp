// Copyright (c) MissiveArts LLC

#include "StateTreeGameActionExistsCondition.h"
#include "MAGameActions/GameAction.h"
#include "VisualLogger/VisualLogger.h"

FStateTreeGameActionExistsCondition::FStateTreeGameActionExistsCondition()
{
	bInvert = false;
}

bool FStateTreeGameActionExistsCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (!IsValid(InstanceData.Owner))
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("%s: invalid owner specified, failing condition"), *Name.ToString());
		return false;
	}

	UGameActionComponent* GAComp = UGameActionComponent::GetGameActionComponentFromActor(InstanceData.Owner);
	if (!IsValid(GAComp))
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("%s: no UGameActionComponent found, failing condition"), *Name.ToString());
		return false;
	}

	bool bActionExists = GAComp->AvailableActions.Contains(InstanceData.ActionName);
	if (bInvert)
	{
		return !bActionExists;
	}

	return bActionExists;
}
