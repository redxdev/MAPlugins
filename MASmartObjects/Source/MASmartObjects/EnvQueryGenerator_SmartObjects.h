// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DataProviders/AIDataProvider.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvQueryGenerator_SmartObjects.generated.h"

/**
 *
 */
UCLASS()
class MASMARTOBJECTS_API UEnvQueryGenerator_SmartObjects : public UEnvQueryGenerator
{
	GENERATED_BODY()

public:
	UEnvQueryGenerator_SmartObjects();

	UPROPERTY(EditAnywhere, Category = Generator)
	FGameplayTagQuery ActivityRequirements;

	UPROPERTY(EditAnywhere, Category = Generator)
	FAIDataProviderFloatValue SearchRadius;

	UPROPERTY(EditAnywhere, Category = Generator)
	TSubclassOf<UEnvQueryContext> SearchCenter;

	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
