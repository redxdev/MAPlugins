// Copyright (c) MissiveArts LLC

#include "AIStateComponent.h"
#include "MAGameplay/MAGameplay.h"
#include "AIController.h"
#include "MAStateTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/StateTreeComponent.h"
#include "VisualLogger/VisualLogger.h"

bool FAIState::IsValid() const
{
	return (Mode == EAIStateMode::BehaviorTree && ::IsValid(BehaviorTree))
		|| (Mode == EAIStateMode::StateTree && StateTree.IsValid());
}

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

	return bResult && InternalApplyState(StateTag, State);
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

bool UAIStateComponent::InternalApplyState(FGameplayTag StateTag, const FAIState& State)
{
	UE_VLOG(GetOwner(), LogMAGameplay, Verbose, TEXT("Moving to AI state %s"), *StateTag.ToString());
	CurrentState = StateTag;

	if (AAIController* Controller = GetOwner<AAIController>())
	{
		if (!CurrentState.IsValid() || !State.IsValid())
		{
			if (UBrainComponent* Brain = Controller->GetBrainComponent())
			{
				Brain->StopLogic(TEXT("Invalid State"));
			}

			return true;
		}
		else
		{
			switch (State.Mode)
			{
			default:
				checkNoEntry();
				return false;

			case EAIStateMode::BehaviorTree:
				{
					UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(Controller->GetBrainComponent());
					if (!BTComp)
					{
						if (Controller->GetBrainComponent())
						{
							UE_VLOG(GetOwner(), LogMAGameplay, Verbose, TEXT("AI State: destroying brain (not a BT component)"));
							Controller->GetBrainComponent()->Cleanup();
							Controller->GetBrainComponent()->DestroyComponent();
						}

						UE_VLOG(GetOwner(), LogMAGameplay, Verbose, TEXT("AI state: creating new BT component"));
						BTComp = NewObject<UBehaviorTreeComponent>(Controller, TEXT("BTComponent"));
						BTComp->RegisterComponent();

						Controller->BrainComponent = BTComp;
					}

					bool bSuccess = true;
					UBlackboardComponent* BlackboardComp = Controller->GetBlackboardComponent();
					if (State.BehaviorTree->BlackboardAsset && (!BlackboardComp || BlackboardComp->IsCompatibleWith(State.BehaviorTree->BlackboardAsset)))
					{
						bSuccess = Controller->UseBlackboard(State.BehaviorTree->BlackboardAsset, BlackboardComp);
					}

					if (bSuccess)
					{
						BTComp->StartTree(*State.BehaviorTree);
					}
					else
					{
						UE_VLOG(GetOwner(), LogMAGameplay, Error, TEXT("AI State: failed to setup behavior tree"));
					}

					return bSuccess;
				}

			case EAIStateMode::StateTree:
				{
					UMAStateTreeComponent* STComp = Cast<UMAStateTreeComponent>(Controller->GetBrainComponent());
					if (!STComp)
					{
						if (Controller->GetBrainComponent())
						{
							UE_VLOG(GetOwner(), LogMAGameplay, Verbose, TEXT("AI State: destroying brain (not a ST component)"));
							Controller->GetBrainComponent()->Cleanup();
							Controller->GetBrainComponent()->DestroyComponent();
						}

						UE_VLOG(GetOwner(), LogMAGameplay, Verbose, TEXT("AI state: creating new ST component"));
						STComp = NewObject<UMAStateTreeComponent>(Controller, TEXT("STComponent"));
						STComp->SetStateTreeRef(State.StateTree);
						STComp->RegisterComponent();

						Controller->BrainComponent = STComp;
					}
					else
					{
						STComp->SetStateTreeRef(State.StateTree);
					}

					STComp->StartLogic();
					return true;
				}
			}
		}
	}

	return false;
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
