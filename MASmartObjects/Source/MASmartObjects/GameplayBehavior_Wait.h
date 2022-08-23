// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameplayBehavior.h"
#include "GameplayBehaviorConfig.h"
#include "GameplayBehavior_Wait.generated.h"

USTRUCT()
struct FWaitBehaviorData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Avatar = nullptr;

	FTimerHandle TimerHandle;

	bool operator==(const FWaitBehaviorData& Other) const { return Avatar == Other.Avatar && TimerHandle == Other.TimerHandle; }
};

UCLASS()
class MASMARTOBJECTS_API UGameplayBehavior_Wait : public UGameplayBehavior
{
	GENERATED_BODY()

public:
	UGameplayBehavior_Wait();

protected:
	virtual bool Trigger(AActor& Avatar, const UGameplayBehaviorConfig* Config, AActor* SmartObjectOwner) override;
	virtual void EndBehavior(AActor& Avatar, const bool bInterrupted) override;

private:
	UPROPERTY()
	TArray<FWaitBehaviorData> ActiveTimers;
};

UCLASS()
class MASMARTOBJECTS_API UGameplayBehaviorConfig_Wait : public UGameplayBehaviorConfig
{
	GENERATED_BODY()

public:
	UGameplayBehaviorConfig_Wait();

	UPROPERTY(Category = Wait, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float WaitTime = 1.f;

	UPROPERTY(Category = Wait, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RandomDeviation = 0.f;
};
