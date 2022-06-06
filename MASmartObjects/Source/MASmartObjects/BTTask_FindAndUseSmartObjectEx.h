﻿// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectSubsystem.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindAndUseSmartObjectEx.generated.h"

UENUM()
enum class ESmartObjectSortMode
{
	First,
	Random,
	Closest,
	Furthest
};

struct FBTUseSOExTaskMemory
{
	TWeakObjectPtr<class UAITask_UseSmartObject> TaskInstance;
};

UCLASS()
class MASMARTOBJECTS_API UBTTask_FindAndUseSmartObjectEx : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindAndUseSmartObjectEx();

	/** Additional tag query to filter available smart objects. We'll query for smart
	 *	objects that support activities tagged in a way matching the filter.
	 *	Note that regular tag-base filtering is going to take place as well */
	UPROPERTY(EditAnywhere, Category = SmartObjects)
	FGameplayTagQuery ActivityRequirements;

	UPROPERTY(EditAnywhere, Category = SmartObjects)
	FBlackboardKeySelector SearchCenter;

	UPROPERTY(EditAnywhere, Category = SmartObjects)
	float Radius;

	UPROPERTY(EditAnywhere, Category = "Search")
	ESmartObjectSortMode SortMode;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTUseSOExTaskMemory); }

	virtual FString GetStaticDescription() const override;

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

private:
	FSmartObjectClaimHandle SortAndClaimSlot(class USmartObjectSubsystem* Subsystem, TArray<FSmartObjectRequestResult>& InResults, const FVector& SearchLocation);
};
