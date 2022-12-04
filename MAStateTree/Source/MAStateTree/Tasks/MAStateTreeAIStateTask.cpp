// Copyright (c) MissiveArts LLC

#include "MAStateTreeAIStateTask.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "MAGameplay/AI/AIStateComponent.h"
#include "MAStateTree/MAStateTree.h"
#include "VisualLogger/VisualLogger.h"

FMAStateTreeApplyAIStateTask::FMAStateTreeApplyAIStateTask()
{
	bApplyStateWhenAlreadyApplied = false;
}

EStateTreeRunStatus FMAStateTreeApplyAIStateTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (!IsValid(InstanceData.Controller))
	{
		UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: invalid controller, failing"), *Name.ToString());
		return EStateTreeRunStatus::Failed;
	}

	if (UAIStateComponent* AIState = InstanceData.Controller->FindComponentByClass<UAIStateComponent>())
	{
		FGameplayTag StateTag = InstanceData.StateTag;
		UE_VLOG(Context.GetOwner(), LogMAStateTree, Verbose, TEXT("%s: applying state '%s'"), *Name.ToString(), *StateTag.ToString());
		if (!bApplyStateWhenAlreadyApplied && StateTag == AIState->GetCurrentStateTag())
		{
			UE_VLOG(Context.GetOwner(), LogMAStateTree, Verbose, TEXT("%s: state already applied, succeeding"), *Name.ToString());
			return EStateTreeRunStatus::Succeeded;
		}

		if (AIState->ApplyState(InstanceData.StateTag))
		{
			return EStateTreeRunStatus::Succeeded;
		}
		else
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	else
	{
		UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: controller '%s' doesn't have a UAIStateComponent, failing"), *Name.ToString(), *GetNameSafe(InstanceData.Controller));
		return EStateTreeRunStatus::Failed;
	}
}
