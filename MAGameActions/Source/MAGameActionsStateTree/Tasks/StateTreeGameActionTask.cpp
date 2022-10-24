// Copyright (c) MissiveArts LLC

#include "StateTreeGameActionTask.h"
#include "StateTreeExecutionContext.h"
#include "MAGameActions/GameAction.h"
#include "VisualLogger/VisualLogger.h"

EStateTreeRunStatus FStateTreeGameActionTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	Reset(InstanceData);

	if (!IsValid(InstanceData.Owner))
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("%s: invalid owner, failing"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	UGameActionComponent* GAComp = UGameActionComponent::GetGameActionComponentFromActor(InstanceData.Owner);
	if (!IsValid(GAComp))
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("%s: no UGameActionComponent found, failing"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.Action = GAComp->ExecuteAction(InstanceData.ActionName);
	if (!InstanceData.Action)
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("%s: failed to execute action with tag %s"), *Name.ToString(), *InstanceData.ActionName.ToString());
		return EStateTreeRunStatus::Failed;
	}

	UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("%s: successfully started action %s"), *GetNameSafe(InstanceData.Action));
	return CheckGameActionState(Context, InstanceData);
}

EStateTreeRunStatus FStateTreeGameActionTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (!IsValid(InstanceData.Action))
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("%s: action was invalidated, failing"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	return CheckGameActionState(Context, InstanceData);
}

void FStateTreeGameActionTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	Reset(InstanceData);
}

void FStateTreeGameActionTask::Reset(FInstanceDataType& InstanceData) const
{
	if (IsValid(InstanceData.Action) && !InstanceData.Action->IsFinished())
	{
		InstanceData.Action->CancelAction();
	}

	InstanceData.Action = nullptr;
}

EStateTreeRunStatus FStateTreeGameActionTask::CheckGameActionState(FStateTreeExecutionContext& Context, FInstanceDataType& InstanceData) const
{
	if (InstanceData.Action->IsFinished())
	{
		if (InstanceData.Action->DidFinishSuccessfully())
		{
			UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("%s: action %s finished successfully"), *Name.ToString(), *GetNameSafe(InstanceData.Action));
			return EStateTreeRunStatus::Succeeded;
		}
		else
		{
			UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("%s: action %s failed"), *Name.ToString(), *GetNameSafe(InstanceData.Action));
			return EStateTreeRunStatus::Failed;
		}
	}

	if (InstanceData.Action->IsCancelled())
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("%s: action %s was cancelled"), *Name.ToString(), *GetNameSafe(InstanceData.Action));
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}
