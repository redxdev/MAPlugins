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
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("Game action task: invalid owner, failing"));
		return EStateTreeRunStatus::Failed;
	}

	UGameActionComponent* GAComp = UGameActionComponent::GetGameActionComponentFromActor(InstanceData.Owner);
	if (!IsValid(GAComp))
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("Game action task: no UGameActionComponent found, failing"));
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.Action = GAComp->ExecuteAction(InstanceData.ActionName);
	if (!InstanceData.Action)
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("Game action task: failed to execute action with tag %s"), *InstanceData.ActionName.ToString());
		return EStateTreeRunStatus::Failed;
	}

	UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("Game action task: successfully started action %s"), *GetNameSafe(InstanceData.Action));
	return CheckGameActionState(Context, InstanceData);
}

EStateTreeRunStatus FStateTreeGameActionTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (!IsValid(InstanceData.Action))
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Warning, TEXT("Game action task: action was invalidated, failing"));
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
			UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("Game action task: action %s finished successfully"), *GetNameSafe(InstanceData.Action));
			return EStateTreeRunStatus::Succeeded;
		}
		else
		{
			UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("Game action task: action %s failed"), *GetNameSafe(InstanceData.Action));
			return EStateTreeRunStatus::Failed;
		}
	}

	if (InstanceData.Action->IsCancelled())
	{
		UE_VLOG(Context.GetOwner(), LogGameActions, Log, TEXT("Game action task: action %s was cancelled"), *GetNameSafe(InstanceData.Action));
		return EStateTreeRunStatus::Failed;
	}

	return EStateTreeRunStatus::Running;
}
