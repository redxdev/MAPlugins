// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "StateTreeTaskBase.h"
#include "MAStateTreeEvaluatorTask.generated.h"

// This is a somewhat hacky task that runs an evaluator inside a task.
// 5.1 removed the ability to have per-state evaluators but these can still be incredibly useful.
// This is potentially very unsafe depending on how each evaluator is implemented.
USTRUCT(meta = (DisplayName = "Run Evaluator"))
struct MASTATETREE_API FMAStateTreeEvaluatorTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FMAStateTreeEvaluatorTask();
	virtual ~FMAStateTreeEvaluatorTask();

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (BaseStruct = "/Script/StateTreeModule.StateTreeEvaluatorBase"))
	FInstancedStruct Evaluator;

protected:
	virtual const UStruct* GetInstanceDataType() const override;

	virtual bool Link(FStateTreeLinker& Linker) override;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_GAMEPLAY_DEBUGGER
	virtual void AppendDebugInfoString(FString& DebugString, const FStateTreeExecutionContext& Context) const override;
#endif

private:

#if WITH_EDITOR
	FDelegateHandle PropChangedDelegateHandle;
	void OnObjectPropertyChanged(UObject* Obj, struct FPropertyChangedEvent& Event);
#endif
};