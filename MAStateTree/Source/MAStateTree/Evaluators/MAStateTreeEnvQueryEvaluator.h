// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "StateTreeEvaluatorBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "MAStateTreeEnvQueryEvaluator.generated.h"

// This is stored in a shared ptr so that we can retain it in a lambda callback from EQS.
// It's unclear if this is the best method - InstanceData can't be captured safely in a lambda as the lifetime isn't guaranteed.
// This was the easiest method I could think of to make sure we retain a pointer to results within the lambda, but
// this should probably be rewritten at some point.
struct FMAStateTreeEnvQueryEvaluatorResultData
{
	uint8 bFinished : 1;
	uint8 bResult : 1;
	TWeakObjectPtr<AActor> ResultActor;
	FVector ResultLocation;
};

USTRUCT()
struct MASTATETREE_API FMAStateTreeEnvQueryEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AActor> Owner = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TObjectPtr<class UEnvQuery> QueryTemplate;

	// TODO: can this be setup so that statetree params can be bound to it
	UPROPERTY(EditAnywhere, Category = Parameter)
	TArray<FEnvNamedValue> QueryConfig;

	UPROPERTY(EditAnywhere, Category = Output)
	FVector ResultLocation = FAISystem::InvalidLocation;

	// TODO: potential for multiple results?
	UPROPERTY(EditAnywhere, Category = Output)
	TWeakObjectPtr<AActor> ResultActor{};

	int32 RequestId = INDEX_NONE;

	UPROPERTY(EditAnywhere, Category = Output)
	uint8 bResult : 1;

	TSharedPtr<FMAStateTreeEnvQueryEvaluatorResultData> ResultData;

	double NextUpdate = 0.0;
};

USTRUCT(meta = (DisplayName = "EQS Evaluator"))
struct MASTATETREE_API FMAStateTreeEnvQueryEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMAStateTreeEnvQueryEvaluatorInstanceData;

	FMAStateTreeEnvQueryEvaluator();

	UPROPERTY(EditAnywhere, Category = Parameter)
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode = EEnvQueryRunMode::SingleResult;

	/** The delay that the evaluator should wait before trying to run the query after a failed attempt */
	UPROPERTY(EditAnywhere, Category = Parameter)
	float RetryCooldown = 1.f;

	/** The default delay that the evaluator should wait before evaluating again */
	UPROPERTY(EditAnywhere, Category = Parameter)
	float TickInterval = 1.f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bClearResultsOnQueryFailure : 1;

	// Event to send when a new result is found.
	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag ResultUpdatedEventTag;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) const override;
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

private:
	void Reset(FStateTreeExecutionContext& Context, FInstanceDataType& InstanceData) const;
	void RunQuery(FStateTreeExecutionContext& Context, FInstanceDataType& InstanceData) const;
};