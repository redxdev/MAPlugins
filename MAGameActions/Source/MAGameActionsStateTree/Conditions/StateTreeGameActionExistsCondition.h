// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "StateTreeGameActionExistsCondition.generated.h"

USTRUCT()
struct MAGAMEACTIONSSTATETREE_API FStateTreeGameActionExistsConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AActor> Owner = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (Categories = "ActionId"))
	FGameplayTag ActionName;
};

USTRUCT(meta = (DisplayName = "Game Action Exists"))
struct MAGAMEACTIONSSTATETREE_API FStateTreeGameActionExistsCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeGameActionExistsConditionInstanceData;

	FStateTreeGameActionExistsCondition();

	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bInvert : 1;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};