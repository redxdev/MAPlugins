// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeTaskBase.h"
#include "MAStateTreeMoveToTask.generated.h"

USTRUCT()
struct MASTATETREE_API FMAStateTreeMoveToTaskInstanceData
{
	GENERATED_BODY()

	FMAStateTreeMoveToTaskInstanceData();

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AActor> Actor;

	UPROPERTY(EditAnywhere, Category = Input, meta = (Optional))
	TObjectPtr<AActor> GoalActor{};

	UPROPERTY(EditAnywhere, Category = Input, meta = (Optional))
	FVector GoalLocation{ForceInit};

	FVector PreviousGoalLocation = FAISystem::InvalidLocation;
	FAIRequestID MoveRequestId = FAIRequestID::InvalidRequest;

	// TODO: need to check if the GC can collect this before we can retrieve the result
	TWeakObjectPtr<class UAITask_MoveTo> MoveTask = nullptr;

	AAIController* GetAIController() const;
};

USTRUCT(meta = (DisplayName = "Move To"))
struct MASTATETREE_API FMAStateTreeMoveToTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using InstanceDataType = FMAStateTreeMoveToTaskInstanceData;

	FMAStateTreeMoveToTask();

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AcceptanceRadius;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TSubclassOf<class UNavigationQueryFilter> FilterClass{};

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "1", UIMin = "1", EditCondition = "bObserveLocation", DisplayAfter = "bObserveLocation"))
	float ObservedLocationTolerance;

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (DisplayAfter = "Actor"))
	uint8 bUseActorForGoal : 1;

	// When true, updates the goal location if it changes.
	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bObserveGoal : 1;

	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bAllowStrafe : 1;

	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bUsePathfinding : 1;

	/** if set, use incomplete path when goal can't be reached */
	UPROPERTY(EditAnywhere, Category = Parameter, AdvancedDisplay)
	uint8 bAllowPartialPath : 1;

	/** if set, path to goal actor will update itself when actor moves */
	UPROPERTY(EditAnywhere, Category = Parameter, AdvancedDisplay)
	uint8 bTrackMovingGoal : 1;

	/** if set, goal location will be projected on navigation data (navmesh) before using */
	UPROPERTY(EditAnywhere, Category = Parameter, AdvancedDisplay)
	uint8 bProjectGoalLocation : 1;

	/** if set, radius of AI's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, Category = Parameter, AdvancedDisplay)
	uint8 bReachTestIncludesAgentRadius : 1;

	/** if set, radius of goal's capsule will be added to threshold between AI and goal location in destination reach test  */
	UPROPERTY(EditAnywhere, Category = Parameter, AdvancedDisplay)
	uint8 bReachTestIncludesGoalRadius : 1;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return InstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

private:
	EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context, FMAStateTreeMoveToTaskInstanceData& InstanceData) const;
	UAITask_MoveTo* PrepareMoveTask(class AAIController& AIController, UAITask_MoveTo* ExistingTask, FAIMoveRequest& MoveReq) const;
};