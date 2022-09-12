// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMATimerTriggerDelegate);

// Configurable timer that can cycle through different periods while looping.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MACOMMON_API UMATimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Resets internal counters, does *not* start the timer again.
	UFUNCTION(BlueprintCallable, Category = Timer)
	void Reset();

	// TODO: initial delay, random initial delay

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
	TArray<float> Timings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
	uint8 bLoopInfinitely : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer, meta = (EditCondition = "!bLoopInfinitely"))
	int32 NumberOfLoops = 1;

	UFUNCTION(BlueprintCallable, Category = Timer)
	FORCEINLINE int32 GetLoopCount() const { return LoopCount; }

	UFUNCTION(BlueprintCallable, Category = Timer)
	FORCEINLINE int32 GetTimingIndex() const { return TimingIndex; }

	UPROPERTY(BlueprintAssignable, Category = Timer)
	FMATimerTriggerDelegate OnTimerTriggered;

private:
	UPROPERTY(VisibleInstanceOnly)
	int32 LoopCount = 0;

	UPROPERTY(VisibleInstanceOnly)
	int32 TimingIndex = 0;

	FTimerHandle ActiveTimer;

	void SetNextTimer();
	void TriggerTimer();
};
