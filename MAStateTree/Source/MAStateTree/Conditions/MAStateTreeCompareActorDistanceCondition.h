// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"
#include "StateTreeConditionBase.h"
#include "MAStateTreeCompareActorDistanceCondition.generated.h"

USTRUCT()
struct MASTATETREE_API FStateTreeCompareActorDistanceConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class AActor> Source = nullptr;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class AActor> Target = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	double Distance = 0.0;
};

USTRUCT(DisplayName = "Distance Compare (Actor to Actor)")
struct MASTATETREE_API FStateTreeCompareActorDistanceCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeCompareActorDistanceConditionInstanceData;

	FStateTreeCompareActorDistanceCondition();

	UPROPERTY(EditAnywhere, Category = Condition)
	uint8 bInvert : 1;

	UPROPERTY(EditAnywhere, Category = Condition)
	EGenericAICheck Operator = EGenericAICheck::Equal;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};