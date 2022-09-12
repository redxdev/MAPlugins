// Copyright (c) MissiveArts LLC

#include "CountingTrigger.h"

void UMACountingTriggerComponent::TriggerPositive()
{
	int32 OldCount = Count++;
	if (OldCount == 0 && DefaultState != ECountingTriggerDefault::Positive)
	{
		OnPositiveTrigger.Broadcast(Count);
	}
	else if (Count == 0 && DefaultState == ECountingTriggerDefault::Neutral)
	{
		OnNeutralTrigger.Broadcast(Count);
	}
}

void UMACountingTriggerComponent::TriggerNegative()
{
	int32 OldCount = Count--;
	if (OldCount == 0 && DefaultState != ECountingTriggerDefault::Negative)
	{
		OnNegativeTrigger.Broadcast(Count);
	}
	else if (Count == 0 && DefaultState == ECountingTriggerDefault::Neutral)
	{
		OnNeutralTrigger.Broadcast(Count);
	}
}

void UMACountingTriggerComponent::Reset()
{
	Count = 0;
}

bool UMACountingTriggerComponent::IsStatePositive() const
{
	return Count > 0 || (DefaultState == ECountingTriggerDefault::Positive && Count == 0);
}

bool UMACountingTriggerComponent::IsStateNegative() const
{
	return Count < 0 || (DefaultState == ECountingTriggerDefault::Negative && Count == 0);
}

bool UMACountingTriggerComponent::IsStateNeutral() const
{
	return Count == 0 && DefaultState == ECountingTriggerDefault::Neutral;
}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

// TODO: test the actual emission of events instead of just states.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCountingTriggerComponentTest, "MACommon.CountingTriggerComponent",
	EAutomationTestFlags::ApplicationContextMask |
	EAutomationTestFlags::CriticalPriority |
	EAutomationTestFlags::ProductFilter)

bool FCountingTriggerComponentTest::RunTest(const FString& Parameters)
{
	UMACountingTriggerComponent* Trigger = NewObject<UMACountingTriggerComponent>();
	if (!TestTrue("Trigger is valid", IsValid(Trigger)))
	{
		return false;
	}

	Trigger->DefaultState = ECountingTriggerDefault::Neutral;
	TestEqual("Count is 0", Trigger->GetCount(), 0);
	TestTrue("IsNeutral (0, default)", Trigger->IsStateNeutral());
	TestFalse("IsPositive (0)", Trigger->IsStatePositive());
	TestFalse("IsNegative, (0)", Trigger->IsStateNegative());

	Trigger->TriggerPositive();
	TestEqual("Count is 1", Trigger->GetCount(), 1);
	TestFalse("IsNeutral (1, default)", Trigger->IsStateNeutral());
	TestTrue("IsPositive (1)", Trigger->IsStatePositive());
	TestFalse("IsNegative (1)", Trigger->IsStateNegative());

	Trigger->TriggerNegative();
	TestEqual("Count is 0", Trigger->GetCount(), 0);
	TestTrue("IsNeutral (0, default)", Trigger->IsStateNeutral());
	TestFalse("IsPositive (0)", Trigger->IsStatePositive());
	TestFalse("IsNegative (0)", Trigger->IsStateNegative());

	Trigger->TriggerNegative();
	TestEqual("Count is -1", Trigger->GetCount(), -1);
	TestFalse("IsNeutral (-1, default)", Trigger->IsStateNeutral());
	TestFalse("IsPositive (-1)", Trigger->IsStatePositive());
	TestTrue("IsNegative (-1)", Trigger->IsStateNegative());

	Trigger->Reset();
	TestEqual("Count is 0", Trigger->GetCount(), 0);

	Trigger->DefaultState = ECountingTriggerDefault::Positive;
	TestFalse("IsNeutral (0)", Trigger->IsStateNeutral());
	TestTrue("IsPositive (0, default)", Trigger->IsStatePositive());
	TestFalse("IsNegative (0)", Trigger->IsStateNegative());

	Trigger->DefaultState = ECountingTriggerDefault::Negative;
	TestFalse("IsNeutral (0)", Trigger->IsStateNeutral());
	TestFalse("IsPositive (0)", Trigger->IsStatePositive());
	TestTrue("IsNegative (0, default)", Trigger->IsStateNegative());

	return true;
}


#endif