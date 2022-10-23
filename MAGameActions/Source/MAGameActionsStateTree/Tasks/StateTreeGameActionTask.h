// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreeTaskBase.h"
#include "StateTreeGameActionTask.generated.h"

USTRUCT()
struct MAGAMEACTIONSSTATETREE_API FStateTreeGameActionTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AActor> Owner = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (Categories = "ActionId"))
	FGameplayTag ActionName;

	UPROPERTY(EditAnywhere, Category = Output)
	TObjectPtr<class UGameAction> Action = nullptr;
};

USTRUCT(meta = (DisplayName = "Game Action"))
struct MAGAMEACTIONSSTATETREE_API FStateTreeGameActionTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeGameActionTaskInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

private:
	void Reset(FInstanceDataType& InstanceData) const;
	EStateTreeRunStatus CheckGameActionState(FStateTreeExecutionContext& Context, FInstanceDataType& InstanceData) const;
};