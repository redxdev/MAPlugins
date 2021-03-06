// Copyright (c) MissiveArts LLC

#include "AIStateComponent.h"
#include "MAGameplay/MAGameplay.h"
#include "AIController.h"
#include "VisualLogger/VisualLogger.h"

void UPawnAIStateComponent::PostInitProperties()
{
	Super::PostInitProperties();

	// Can't use UCLASS(Within = ) because that prevents you from being able to add this component via the editor for some reason.
	if (GetOwner() && !GetOwner<APawn>())
	{
		UE_LOG(LogMAGameplay, Error, TEXT("%s is not an APawn, cannot own UPawnAIStateComponent %s"), *GetNameSafe(GetOwner()), *GetName());
	}
}

bool UPawnAIStateComponent::ApplyState(FGameplayTag StateTag)
{
	if (UAIStateComponent* AIComp = GetAIStateComponent())
	{
		return AIComp->ApplyState(StateTag);
	}

	return false;
}

bool UPawnAIStateComponent::CancelState()
{
	if (UAIStateComponent* AIComp = GetAIStateComponent())
	{
		return AIComp->CancelState();
	}

	return false;
}

bool UPawnAIStateComponent::ReapplyActiveState()
{
	if (UAIStateComponent* AIComp = GetAIStateComponent())
	{
		return AIComp->ReapplyActiveState();
	}

	return false;
}

UAIStateComponent* UPawnAIStateComponent::GetAIStateComponent() const
{
	if (APawn* Pawn = GetOwner<APawn>())
	{
		if (AAIController* Controller = Pawn->GetController<AAIController>())
		{
			return Controller->FindComponentByClass<UAIStateComponent>();
		}
	}

	return nullptr;
}

UAIStateComponent::UAIStateComponent()
{
	bResetStateWhenUnpossessed = true;
}

bool UAIStateComponent::ApplyState(FGameplayTag StateTag)
{
	bool bResult = true;
	FAIState State;
	if (StateTag.IsValid() && !FindState(StateTag, State))
	{
		StateTag = FGameplayTag::EmptyTag;
		bResult = false;
	}

	InternalApplyState(StateTag, State);
	return bResult;
}

bool UAIStateComponent::CancelState()
{
	return ApplyState(FGameplayTag::EmptyTag);
}


bool UAIStateComponent::ReapplyActiveState()
{
	return ApplyState(CurrentState);
}

void UAIStateComponent::PostInitProperties()
{
	Super::PostInitProperties();

	// Can't use UCLASS(Within = ) because that prevents you from being able to add this component via the editor for some reason.
	if (GetOwner() && !GetOwner<AAIController>())
	{
		UE_LOG(LogMAGameplay, Error, TEXT("%s is not an AAIController, cannot own UAIStateComponent %s"), *GetNameSafe(GetOwner()), *GetName());
	}
}

void UAIStateComponent::BeginPlay()
{
	Super::BeginPlay();

	AAIController* Controller = GetOwner<AAIController>();
	if (ensure(Controller))
	{
		Controller->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		OnPossessedPawnChanged(nullptr, Controller->GetPawn());
	}
}

void UAIStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AAIController* Controller = GetOwner<AAIController>())
	{
		Controller->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::OnPossessedPawnChanged);
	}

	Super::EndPlay(EndPlayReason);
}

bool UAIStateComponent::FindState(FGameplayTag StateTag, FAIState& OutState)
{
	if (!StateTag.IsValid())
	{
		return false;
	}

	if (UPawnAIStateComponent* PawnComp = PawnAIState.Get())
	{
		if (FAIState* Found = PawnComp->States.Find(StateTag))
		{
			OutState = *Found;
			return true;
		}
	}

	if (FAIState* Found = States.Find(StateTag))
	{
		OutState = *Found;
		return true;
	}

	return false;
}

void UAIStateComponent::InternalApplyState(FGameplayTag StateTag, const FAIState& State)
{
	UE_VLOG(GetOwner(), LogMAGameplay, Verbose, TEXT("Moving to AI state %s"), *StateTag.ToString());
	CurrentState = StateTag;

	if (AAIController* Controller = GetOwner<AAIController>())
	{
		if (!CurrentState.IsValid() || !IsValid(State.BehaviorTree))
		{
			if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent()))
			{
				BTComp->StopTree();
			}
		}
		else
		{
			Controller->RunBehaviorTree(State.BehaviorTree);
		}
	}
}

void UAIStateComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	PawnAIState = nullptr;
	if (IsValid(NewPawn))
	{
		ResolvePawnAIState(NewPawn);

		if (!CurrentState.IsValid())
		{
			if (UPawnAIStateComponent* PawnComp = PawnAIState.Get())
			{
				if (PawnComp->DefaultState.IsValid())
				{
					ApplyState(PawnComp->DefaultState);
				}
			}
		}

		if (!CurrentState.IsValid() && DefaultState.IsValid())
		{
			ApplyState(DefaultState);
		}
	}
	else
	{
		if (bResetStateWhenUnpossessed)
		{
			ApplyState(FGameplayTag::EmptyTag);
		}
	}
}

void UAIStateComponent::ResolvePawnAIState(APawn* NewPawn)
{
	check(NewPawn);
	PawnAIState = NewPawn->FindComponentByClass<UPawnAIStateComponent>();
}
