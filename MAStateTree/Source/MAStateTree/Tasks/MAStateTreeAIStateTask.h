// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MAStateTreeAIStateTask.generated.h"

USTRUCT()
struct MASTATETREE_API FMAStateTreeApplyAIStateTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> Controller = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (Categories = "AI.State"))
	FGameplayTag StateTag;
};

// Applies an AI state to a target controller.
// WARNING: If the target controller is the controller running the current state tree, this can halt or restart execution of the current state tree, meaning you won't be able to handle
// any transitions from this task when a new state is applied.
USTRUCT(meta = (DisplayName = "Apply AI State"))
struct MASTATETREE_API FMAStateTreeApplyAIStateTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMAStateTreeApplyAIStateTaskInstanceData;

	FMAStateTreeApplyAIStateTask();

	// If true, applies a state even when it is already current.
	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bApplyStateWhenAlreadyApplied : 1;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};