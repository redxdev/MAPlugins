// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeComponent.h"
#include "MAStateTreeComponent.generated.h"

// This class exists because UStateTreeComponent doesn't expose StateTreeRef which we need for AIStateComponents to work correctly.
UCLASS(ClassGroup = AI, HideCategories = (Activation, Collision))
class MAGAMEPLAY_API UMAStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	UMAStateTreeComponent();

	// Changes out the state tree, stopping logic if it is running.
	void SetStateTreeRef(const FStateTreeReference& InStateTreeRef);
};
