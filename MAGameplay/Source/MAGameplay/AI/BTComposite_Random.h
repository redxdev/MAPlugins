// Copyright (c) MissiveArts LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTComposite_Random.generated.h"

struct FBTRandomCompositeMemory : public FBTCompositeMemory
{
	TSet<int32> FailedChildren;
};

/**
 * Random composite node.
 * Acts like a selector except the order of child node execution is random. Keeps executing random children until one succeeds.
 * Weight is 1.0 for every child by default. Weights can be overridden with a "RandomWeight" decorator on the child.
 */
UCLASS()
class MAGAMEPLAY_API UBTComposite_Random : public UBTCompositeNode
{
	GENERATED_BODY()

public:
	UBTComposite_Random();

	// If true, when a child fails it will not be considered again until this composite is exited.
	UPROPERTY(EditAnywhere, Category = Composite)
	uint8 bExcludeFailedChildren : 1;

	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	virtual void NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const override;
};

/**
 * Gives the node a weight for use with UBTComposite_Random. Only the first decorator of this type will be used.
 */
UCLASS()
class MAGAMEPLAY_API UBTDecorator_RandomWeight : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_RandomWeight();

	// If weight is <= 0 then this node is ignored by the random composite.
	UPROPERTY(EditAnywhere, Category = Decorator, meta = (ClampMin = 0))
	float Weight = 1.0f;

	virtual FString GetStaticDescription() const override;
};