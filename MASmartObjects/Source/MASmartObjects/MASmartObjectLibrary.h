// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectRuntime.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MASmartObjectLibrary.generated.h"

UCLASS()
class MASMARTOBJECTS_API UMASmartObjectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SmartObject")
	static FSmartObjectClaimHandle GetValueAsSOClaimHandle(UBlackboardComponent* BlackboardComponent, const FBlackboardKeySelector& KeySelector);
};
