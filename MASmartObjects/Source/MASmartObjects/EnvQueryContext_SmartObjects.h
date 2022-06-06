// Copyright (c) MissiveArts LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectSubsystem.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_SmartObjects.generated.h"

UCLASS(Abstract, Blueprintable)
class MASMARTOBJECTS_API UEnvQueryContext_SmartObjects : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	UEnvQueryContext_SmartObjects();

	UPROPERTY(EditAnywhere, Category = Context)
	FGameplayTagQuery ActivityRequirements;

	UPROPERTY(EditAnywhere, Category = Context)
	float SearchRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = Context)
	TSubclassOf<UEnvQueryContext> SearchCenter;

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
