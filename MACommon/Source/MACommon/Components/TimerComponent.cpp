// Copyright (c) MissiveArts LLC

#include "TimerComponent.h"
#include "Misc/UObjectToken.h"

#define LOCTEXT_NAMESPACE "MATimerComponent"

void UMATimerComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	check(GetWorld());

	if (bReset)
	{
		Reset();
	}

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerPaused(ActiveTimer))
	{
		TimerManager.UnPauseTimer(ActiveTimer);
	}
	else if (!TimerManager.IsTimerActive(ActiveTimer))
	{
		SetNextTimer();
	}
}

void UMATimerComponent::Deactivate()
{
	check(GetWorld());

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(ActiveTimer))
	{
		TimerManager.PauseTimer(ActiveTimer);
	}

	Super::Deactivate();
}

void UMATimerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	check(GetWorld());

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(ActiveTimer);
}

void UMATimerComponent::Reset()
{
	check(GetWorld());

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(ActiveTimer);

	LoopCount = 0;
	TimingIndex = 0;
}

void UMATimerComponent::SetNextTimer()
{
	check(GetWorld());

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(ActiveTimer);

	if (Timings.Num() == 0)
	{
		FMessageLog("PIE").Warning()
			->AddToken(FTextToken::Create(LOCTEXT("Timer", "Timer")))
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(LOCTEXT("IsMissingTimings", "has 0 timings available.")));
		return;
	}

	if (TimingIndex >= Timings.Num())
	{
		if (!bLoopInfinitely && LoopCount >= NumberOfLoops)
		{
			return;
		}

		++LoopCount;
		TimingIndex = 0;
	}

	float NewTiming = Timings[TimingIndex];
	if (NewTiming <= 0)
	{
		ActiveTimer = TimerManager.SetTimerForNextTick(this, &ThisClass::TriggerTimer);
	}
	else
	{
		TimerManager.SetTimer(ActiveTimer, this, &ThisClass::TriggerTimer, NewTiming);
	}
}

void UMATimerComponent::TriggerTimer()
{
	OnTimerTriggered.Broadcast();

	++TimingIndex;
	SetNextTimer();
}

#undef LOCTEXT_NAMESPACE
