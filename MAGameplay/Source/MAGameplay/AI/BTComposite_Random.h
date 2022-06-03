// Copyright (c) MissiveArts LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BTComposite_Random.generated.h"

/**
 * Random composite node.
 * Acts like a selector except the order of child node execution is random. Keeps executing random children until one succeeds.
 */
UCLASS()
class MAGAMEPLAY_API UBTComposite_Random : public UBTCompositeNode
{
	GENERATED_BODY()

public:
	UBTComposite_Random();

	virtual int32 GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;
};
