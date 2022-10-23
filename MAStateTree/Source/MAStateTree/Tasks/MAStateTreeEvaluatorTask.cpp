// Copyright (c) MissiveArts LLC

#include "MAStateTreeEvaluatorTask.h"
#include "StateTreeDelegates.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "MAStateTree/MAStateTree.h"
#include "VisualLogger/VisualLogger.h"

#if WITH_EDITOR
#include "StateTreeEditorData.h"
#endif

FMAStateTreeEvaluatorTask::FMAStateTreeEvaluatorTask()
{
#if WITH_EDITOR
	PropChangedDelegateHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FMAStateTreeEvaluatorTask::OnObjectPropertyChanged);
#endif
}

FMAStateTreeEvaluatorTask::~FMAStateTreeEvaluatorTask()
{
#if WITH_EDITOR
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(PropChangedDelegateHandle);
#endif
}

const UStruct* FMAStateTreeEvaluatorTask::GetInstanceDataType() const
{
	if (Evaluator.IsValid())
	{
		const FStateTreeEvaluatorBase& EvaluatorInst = Evaluator.Get<FStateTreeEvaluatorBase>();
		return EvaluatorInst.GetInstanceDataType();
	}

	return nullptr;
}

bool FMAStateTreeEvaluatorTask::Link(FStateTreeLinker& Linker)
{
	if (Evaluator.IsValid())
	{
		FStateTreeEvaluatorBase& EvaluatorInst = Evaluator.GetMutable<FStateTreeEvaluatorBase>();
		EvaluatorInst.BindingsBatch = BindingsBatch;
		EvaluatorInst.DataViewIndex = DataViewIndex;
		EvaluatorInst.InstanceIndex = InstanceIndex;
		EvaluatorInst.bInstanceIsObject = bInstanceIsObject;
		return EvaluatorInst.Link(Linker);
	}

	UE_LOG(LogMAStateTree, Error, TEXT("%s: no evaluator instance specified"), *Name.ToString());
	return false;
}

EStateTreeRunStatus FMAStateTreeEvaluatorTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Evaluator.IsValid())
	{
		const FStateTreeEvaluatorBase& EvaluatorInst = Evaluator.Get<FStateTreeEvaluatorBase>();
		EvaluatorInst.TreeStart(Context);
		return EStateTreeRunStatus::Running;
	}

	UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: invalid evaluator instance, failing EnterState"), *Name.ToString());
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FMAStateTreeEvaluatorTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	if (Evaluator.IsValid())
	{
		const FStateTreeEvaluatorBase& EvaluatorInst = Evaluator.Get<FStateTreeEvaluatorBase>();
		EvaluatorInst.Tick(Context, DeltaTime);
		return EStateTreeRunStatus::Running;
	}

	UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: invalid evaluator instance, failing tick"), *Name.ToString());
	return EStateTreeRunStatus::Failed;
}

void FMAStateTreeEvaluatorTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	if (Evaluator.IsValid())
	{
		const FStateTreeEvaluatorBase& EvaluatorInst = Evaluator.Get<FStateTreeEvaluatorBase>();
		EvaluatorInst.TreeStop(Context);
		return;
	}

	UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("%s: invalid evaluator instance, ignoring ExitState"), *Name.ToString());
}

void FMAStateTreeEvaluatorTask::AppendDebugInfoString(FString& DebugString, const FStateTreeExecutionContext& Context) const
{
	if (Evaluator.IsValid())
	{
		const FStateTreeEvaluatorBase& EvaluatorInst = Evaluator.Get<FStateTreeEvaluatorBase>();
		EvaluatorInst.AppendDebugInfoString(DebugString, Context);
	}
	else
	{
		FStateTreeTaskCommonBase::AppendDebugInfoString(DebugString, Context);
	}
}

#if WITH_EDITOR
void FMAStateTreeEvaluatorTask::OnObjectPropertyChanged(UObject* Obj, FPropertyChangedEvent& Event)
{
	// This is hacky, but we need a way to get notified when the evaluator class changes in the
	// editor (so that we can update the instance data for the compiler) and StateTree doesn't provide
	// any callbacks for this stuff, not even a pre-compile event.
	// Sooooo we need to listen for property changes to UStateTreeState in order to update the type of the instance
	// data for this task.
	// There doesn't seem to be a Chain version of this event either so this function ends up running much more often
	// than necessary.
	// Ideally StateTree would fixup instances during compilation rather than just failing... in which case you'd need
	// to compile before editing the evaluator's instance data. But sadly it doesn't, it just outright fails compilation.
	UStateTreeState* State = Cast<UStateTreeState>(Obj);
	if (!State || Event.GetPropertyName() != GET_MEMBER_NAME_CHECKED(FMAStateTreeEvaluatorTask, Evaluator))
	{
		return;
	}

	auto UpdateNode = [this, State](FStateTreeEditorNode& Node)
	{
		if (Node.Node.IsValid() &&
			Node.Node.GetScriptStruct()->IsChildOf(FMAStateTreeEvaluatorTask::StaticStruct()) &&
			Node.Node.GetPtr<FMAStateTreeEvaluatorTask>() == this)
		{
			if (const UScriptStruct* InstanceType = Cast<const UScriptStruct>(GetInstanceDataType()))
			{
				if (Node.Instance.GetScriptStruct() != InstanceType)
				{
					Node.Instance.InitializeAs(InstanceType);
					Node.InstanceObject = nullptr;
				}
			}
			else if (const UClass* InstanceClass = Cast<const UClass>(GetInstanceDataType()))
			{
				if (!Node.InstanceObject || Node.InstanceObject->GetClass() != InstanceClass)
				{
					Node.Instance.Reset();
					Node.InstanceObject = NewObject<UObject>(State->GetTypedOuter<UStateTreeEditorData>(), InstanceClass);
				}
			}
			else
			{
				Node.Instance.Reset();
				Node.InstanceObject = nullptr;
			}

			return EStateTreeVisitor::Break;
		}

		return EStateTreeVisitor::Continue;
	};


	if (UpdateNode(State->SingleTask) == EStateTreeVisitor::Break)
	{
		return;
	}

	for (FStateTreeEditorNode& Node : State->Tasks)
	{
		if (UpdateNode(Node) == EStateTreeVisitor::Break)
		{
			return;
		}
	}
}
#endif