// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CountingTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCountingTriggerDelegate, int32, NewCount);

UENUM()
enum class ECountingTriggerDefault : uint8
{
	Negative,
	Neutral,
	Positive,
};

// A utility that tracks "positive" and "negative" triggers, only firing further events when the number of one side
// is greater than the number of the other. Also supports a "neutral" event that is fired when the number of positive/negative triggers
// are equal.
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class MACOMMON_API UMACountingTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Counting Trigger")
	FCountingTriggerDelegate OnPositiveTrigger;

	UPROPERTY(BlueprintAssignable, Category = "Counting Trigger")
	FCountingTriggerDelegate OnNegativeTrigger;

	// Only called when DefaultValue is Neutral and the count hits 0.
	UPROPERTY(BlueprintAssignable, Category = "Counting Trigger")
	FCountingTriggerDelegate OnNeutralTrigger;

	// The default state this trigger is in when the count is 0.
	UPROPERTY(EditAnywhere, Category = "Counting Trigger")
	ECountingTriggerDefault DefaultState = ECountingTriggerDefault::Neutral;

	UFUNCTION(BlueprintCallable, Category = "Counting Trigger")
	void TriggerPositive();

	UFUNCTION(BlueprintCallable, Category = "Counting Trigger")
	void TriggerNegative();

	// Resets the internal counter. Does not fire any events.
	UFUNCTION(BlueprintCallable, Category = "Counting Trigger")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "Counting Trigger")
	bool IsStatePositive() const;

	UFUNCTION(BlueprintCallable, Category = "Counting Trigger")
	bool IsStateNegative() const;

	UFUNCTION(BlueprintCallable, Category = "Counting Trigger")
	bool IsStateNeutral() const;

	// Gets the internal counter.
	UFUNCTION(BlueprintCallable, Category = "Counting Trigger")
	FORCEINLINE int32 GetCount() const { return Count; }

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Counting Trigger")
	int32 Count = 0;
};