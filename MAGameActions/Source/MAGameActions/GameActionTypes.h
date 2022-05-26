// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameActionTypes.generated.h"

USTRUCT(BlueprintType)
struct MAGAMEACTIONS_API FGameActionHandle
{
    GENERATED_BODY()
	
    friend class UGameActionComponent;

    FGameActionHandle();
    FGameActionHandle(uint32 Id);
	
    static const FGameActionHandle Invalid;

    FORCEINLINE bool IsValid() const
    {
        return Id > 0;
    }

    friend bool operator==(const FGameActionHandle& Lhs, const FGameActionHandle& Rhs)
    {
        return Lhs.Id == Rhs.Id;
    }

    friend bool operator!=(const FGameActionHandle& Lhs, const FGameActionHandle& Rhs)
    {
        return !(Lhs == Rhs);
    }

private:
    uint32 Id;
};

UCLASS()
class MAGAMEACTIONS_API UGameActionHandleLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Game Actions")
    static bool IsValid(const FGameActionHandle& Handle);

    UFUNCTION(BlueprintPure, Category = "Game Actions")
    static FGameActionHandle GetInvalidHandle();
};