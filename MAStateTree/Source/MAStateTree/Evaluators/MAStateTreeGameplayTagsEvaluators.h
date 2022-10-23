// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "MAStateTreeGameplayTagsEvaluators.generated.h"

USTRUCT()
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UObject> Target;

	UPROPERTY(EditAnywhere, Category = Output)
	FGameplayTagContainer Tags;
};

USTRUCT(meta = (DisplayName = "Gameplay Tag Interface Evaluator"))
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceEvaluator : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceData = FMAStateTreeGameplayTagInterfaceEvaluatorInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceData::StaticStruct(); }

	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void TreeStop(FStateTreeExecutionContext& Context) const override;
};