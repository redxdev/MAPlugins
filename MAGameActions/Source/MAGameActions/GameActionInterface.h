// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameActionInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UGameActionInterface : public UInterface
{
    GENERATED_BODY()
};

class MAGAMEACTIONS_API IGameActionInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Game Actions")
    virtual class UGameActionComponent* GetGameActionComponent() const;
};