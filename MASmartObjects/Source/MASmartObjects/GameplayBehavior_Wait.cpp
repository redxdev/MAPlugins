// Copyright (c) MissiveArts LLC


#include "GameplayBehavior_Wait.h"

UGameplayBehavior_Wait::UGameplayBehavior_Wait()
{
}

bool UGameplayBehavior_Wait::Trigger(AActor& Avatar, const UGameplayBehaviorConfig* Config, AActor* SmartObjectOwner)
{
	const UGameplayBehaviorConfig_Wait* WaitConfig = CastChecked<UGameplayBehaviorConfig_Wait>(Config);
	if (!WaitConfig)
	{
		return false;
	}

	UWorld* World = Avatar.GetWorld();
	if (!World)
	{
		return false;
	}

	float Time = WaitConfig->WaitTime;
	if (WaitConfig->RandomDeviation > 0.f)
	{
		Time += FMath::FRandRange(0.f, WaitConfig->RandomDeviation);
	}

	if (Time <= 0.f)
	{
		return false;
	}

	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(
		Handle,
		FTimerDelegate::CreateWeakLambda(this, [this, AvatarPtr=&Avatar] { EndBehavior(*AvatarPtr, false); }),
		Time,
		false);

	if (!Handle.IsValid())
	{
		return false;
	}

	FWaitBehaviorData Data;
	Data.Avatar = &Avatar;
	Data.TimerHandle = Handle;

	ActiveTimers.Add(Data);
	return true;
}

void UGameplayBehavior_Wait::EndBehavior(AActor& Avatar, const bool bInterrupted)
{
	FWaitBehaviorData* Data = ActiveTimers.FindByPredicate([AvatarPtr=&Avatar](const FWaitBehaviorData& Entry) { return Entry.Avatar == AvatarPtr; });
	if (Data)
	{
		UWorld* World = Avatar.GetWorld();
		if (World && Data->TimerHandle.IsValid())
		{
			World->GetTimerManager().ClearTimer(Data->TimerHandle);
		}

		ActiveTimers.RemoveSingleSwap(*Data, false);
	}

	Super::EndBehavior(Avatar, bInterrupted);
}

UGameplayBehaviorConfig_Wait::UGameplayBehaviorConfig_Wait()
{
	BehaviorClass = UGameplayBehavior_Wait::StaticClass();
}
