// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "MAStateTreeGameplayTagsConditions.generated.h"

USTRUCT()
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceMatchConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UObject> Target = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag Tag;
};

USTRUCT(meta = (DisplayName = "Gameplay Tag Match (Interface)"))
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceMatchCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMAStateTreeGameplayTagInterfaceMatchConditionInstanceData;

	FMAStateTreeGameplayTagInterfaceMatchCondition();

	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bInvert : 1;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

USTRUCT()
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceContainerMatchConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UObject> Target = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTagContainer TagContainer;
};

USTRUCT(meta = (DisplayName = "Gameplay Tag Container Match (Interface)"))
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceContainerMatchCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMAStateTreeGameplayTagInterfaceContainerMatchConditionInstanceData;

	FMAStateTreeGameplayTagInterfaceContainerMatchCondition();

	UPROPERTY(EditAnywhere, Category = Parameter)
	EGameplayContainerMatchType MatchType = EGameplayContainerMatchType::Any;

	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bInvert : 1;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

USTRUCT()
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceQueryMatchConditionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UObject> Target = nullptr;
};

USTRUCT(meta = (DisplayName = "Gameplay Tag Query (Interface)"))
struct MASTATETREE_API FMAStateTreeGameplayTagInterfaceQueryMatchCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMAStateTreeGameplayTagInterfaceQueryMatchConditionInstanceData;

	FMAStateTreeGameplayTagInterfaceQueryMatchCondition();

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTagQuery TagQuery;

	UPROPERTY(EditAnywhere, Category = Parameter)
	uint8 bInvert : 1;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};