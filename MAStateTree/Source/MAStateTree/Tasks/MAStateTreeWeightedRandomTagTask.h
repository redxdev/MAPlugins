// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "MAStateTreeWeightedRandomTagTask.generated.h"

USTRUCT()
struct MASTATETREE_API FMAStateTreeWeightedRandomTagTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "1.0"))
	TMap<FGameplayTag, float> TagWeights;

	UPROPERTY(EditAnywhere, Category = Output)
	FGameplayTag ChosenTag;
};

USTRUCT(meta = (DisplayName = "Select Weighted Random Tag"))
struct MASTATETREE_API FMAStateTreeWeightedRandomTagTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMAStateTreeWeightedRandomTagTaskInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};