// Copyright (c) MissiveArts LLC


#include "GameActionComponent.h"
#include "GameAction.h"
#include "GameActionInterface.h"
#include "MACommon/ObjectReferencer.h"
#include "MACommon/Coroutines/LatentUtilities.h"
#include "UE5Coro/LatentAwaiters.h"
#include "VisualLogger/VisualLogger.h"

UGameActionComponent::UGameActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	NextActionId = 1;
}

void UGameActionComponent::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<UGameAction>& ActionClass : InitialActions)
	{
		FGameActionHandle Handle;
		BeginExecuteAction(ActionClass, Handle);
	}
}

void UGameActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CancelAllActions();
}

void UGameActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TickCooldowns(DeltaTime);
	TickActions(DeltaTime);
}

void UGameActionComponent::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = FGameplayTagContainer();
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Action)
		{
			TagContainer.AppendTags(Action->OwnedTags);
		}
	}
}

bool UGameActionComponent::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Action && Action->OwnedTags.HasTag(TagToCheck))
		{
			return true;
		}
	}

	return false;
}

bool UGameActionComponent::IsActionActive(const FGameActionHandle& Handle) const
{
	return ActiveActions.Contains(Handle.Id);
}

UGameAction* UGameActionComponent::GetActiveAction(const FGameActionHandle& Handle) const
{
	if (!Handle.IsValid())
	{
		return nullptr;
	}

	TObjectPtr<UGameAction> const* Action =  ActiveActions.Find(Handle.Id);
	if (Action != nullptr)
	{
		return *Action;
	}

	return nullptr;
}

void UGameActionComponent::GetActiveActionHandles(TArray<FGameActionHandle>& ActionHandles) const
{
	for (auto& Elem : ActiveActions)
	{
		ActionHandles.Add(FGameActionHandle(Elem.Key));
	}
}

UGameAction* UGameActionComponent::GetActiveActionByClass(TSubclassOf<UGameAction> ActionClass) const
{
	if (!ActionClass)
	{
		return nullptr;
	}
	
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Action && Action->IsA(ActionClass))
		{
			return Action;
		}
	}

	return nullptr;
}

void UGameActionComponent::GetActiveActionHandlesByClass(TSubclassOf<UGameAction> ActionClass, TArray<FGameActionHandle>& ActionHandles) const
{
	if (!ActionClass)
	{
		return;
	}

	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Action && Action->IsA(ActionClass))
		{
			ActionHandles.Add(Elem.Key);
		}
	}
}

UGameAction* UGameActionComponent::ExecuteAction(FGameplayTag Name)
{
	FGameActionHandle Handle;
	return BeginExecuteAction(Name, Handle);
}

UGameAction* UGameActionComponent::ExecuteAction(FGameplayTag Name, FGameActionHandle& OutHandle)
{
	return BeginExecuteAction(Name, OutHandle);
}

UGameAction* UGameActionComponent::ExecuteActionByClass(TSubclassOf<UGameAction> ActionClass)
{
	FGameActionHandle Handle;
	return BeginExecuteAction(ActionClass, Handle);
}

UGameAction* UGameActionComponent::ExecuteActionByClass(TSubclassOf<UGameAction> ActionClass, FGameActionHandle& OutHandle)
{
	return BeginExecuteAction(ActionClass, OutHandle);
}

FAsyncCoroutine UGameActionComponent::ExecuteActionLatent(FGameplayTag Name, FLatentActionInfo LatentInfo, EGameActionExecutionResult& ActionResult)
{
	check(GetWorld());
	
	FGameActionHandle Handle = FGameActionHandle::Invalid;
	UGameAction* Action = BeginExecuteAction(Name, Handle);
	if (!Action)
	{
		ActionResult = EGameActionExecutionResult::Failed;
		Handle = FGameActionHandle::Invalid;
	}
	else
	{
		// This can happen if BeginAction calls cancel or end immediately
		if (!IsActionActive(Handle))
		{
			Handle = FGameActionHandle::Invalid;
				
			if (Action->IsCancelled())
			{
				ActionResult = EGameActionExecutionResult::Cancelled;
			}
			else
			{
				ActionResult = Action->DidFinishSuccessfully() ? EGameActionExecutionResult::Succeeded : EGameActionExecutionResult::Failed;
			}

			co_return;
		}
	}

	co_await InternalExecuteActionLatent(Handle, Action, ActionResult);
}

FAsyncCoroutine UGameActionComponent::ExecuteActionByClassLatent(TSubclassOf<UGameAction> ActionClass, FLatentActionInfo LatentInfo, EGameActionExecutionResult& ActionResult)
{
	check(GetWorld());
	
	FGameActionHandle Handle = FGameActionHandle::Invalid;
	UGameAction* Action = BeginExecuteAction(ActionClass, Handle);
	if (!Action)
	{
		ActionResult = EGameActionExecutionResult::Failed;
		Handle = FGameActionHandle::Invalid;
	}
	else
	{
		// This can happen if BeginAction calls cancel or end immediately
		if (!IsActionActive(Handle))
		{
			Handle = FGameActionHandle::Invalid;
				
			if (Action->IsCancelled())
			{
				ActionResult = EGameActionExecutionResult::Cancelled;
			}
			else
			{
				ActionResult = Action->DidFinishSuccessfully() ? EGameActionExecutionResult::Succeeded : EGameActionExecutionResult::Failed;
			}

			co_return;
		}
	}

	co_await InternalExecuteActionLatent(Handle, Action, ActionResult);
}

bool UGameActionComponent::CancelAction(const FGameActionHandle& Handle)
{
	return CancelActionInternal(Handle, true);
}

int32 UGameActionComponent::CancelActionsByTag(const FGameplayTagQuery Query)
{
	TArray<FGameActionHandle> Handles;
	if (!FindActionsByTag(Query, Handles))
	{
		return 0;
	}

	return CancelActionHandles(Handles);
}

int32 UGameActionComponent::CancelActionsByClass(TSubclassOf<UGameAction> ActionClass)
{
	TArray<FGameActionHandle> Handles;
	if (!FindActionsByClass(ActionClass, Handles))
	{
		return 0;
	}

	return CancelActionHandles(Handles);
}

int32 UGameActionComponent::CancelAllActions()
{
	TArray<FGameActionHandle> Handles;
	GetActiveActionHandles(Handles);

	return CancelActionHandles(Handles);
}

void UGameActionComponent::NotifyActiveActions(FGameplayTag Tag, UObject* Notify, TSubclassOf<UGameAction> ActionClass)
{
	if (!ActionClass)
	{
		ActionClass = UGameAction::StaticClass();
	}
	
	TArray<UGameAction*> ActionsToNotify;
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (!ActionClass || Action->IsA(ActionClass))
		{
			ActionsToNotify.Add(Action);
		}
	}

	for (UGameAction* Action : ActionsToNotify)
	{
		Action->OnNotifyAction(Tag, Notify);
	}
}

void UGameActionComponent::NotifyActiveActionsFromMontage(FGameplayTag Tag, UObject* Notify, UAnimSequenceBase* Montage)
{
	if (!Montage)
	{
		return;
	}
	
	TArray<UGameAction*> ActionsToNotify;
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Action->IsPlayingMontage(Montage))
		{
			ActionsToNotify.Add(Action);
		}
	}

	for (UGameAction* Action : ActionsToNotify)
	{
		Action->OnNotifyAction(Tag, Notify);
	}
}

bool UGameActionComponent::FindActionsByTag(FGameplayTagQuery Query, TArray<FGameActionHandle>& OutHandles) const
{
	bool bFoundAny = false;
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Query.Matches(Action->OwnedTags))
		{
			bFoundAny = true;
			OutHandles.Add(Elem.Key);
		}
	}

	return bFoundAny;
}

bool UGameActionComponent::ContainsActionWithTag(FGameplayTagQuery Query) const
{
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Query.Matches(Action->OwnedTags))
		{
			return true;
		}
	}

	return false;
}

bool UGameActionComponent::FindActionsByClass(TSubclassOf<UGameAction> ActionClass, TArray<FGameActionHandle>& OutHandles) const
{
	if (!ActionClass)
	{
		return false;
	}

	bool bFoundAny = false;
	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Action->IsA(ActionClass))
		{
			bFoundAny = true;
			OutHandles.Add(Elem.Key);
		}
	}

	return bFoundAny;
}

bool UGameActionComponent::ContainsActionOfClass(TSubclassOf<UGameAction> ActionClass) const
{
	if (!ActionClass)
	{
		return false;
	}

	for (auto& Elem : ActiveActions)
	{
		UGameAction* Action = Elem.Value;
		if (Action->IsA(ActionClass))
		{
			return true;
		}
	}

	return false;
}

void UGameActionComponent::ApplyCooldown(FGameplayTag Tag, float CooldownTime)
{
	FCooldownData Cooldown;
	Cooldown.InitialTime = CooldownTime;
	Cooldown.TimeLeft = CooldownTime;
	ActiveCooldowns.Add(Tag, Cooldown);
}

float UGameActionComponent::GetCooldownTimeLeft(FGameplayTag Tag) const
{
	if (!ActiveCooldowns.Contains(Tag))
	{
		return -1.f;
	}

	return ActiveCooldowns[Tag].TimeLeft;
}

float UGameActionComponent::GetCooldownInitialTime(FGameplayTag Tag) const
{
	if (!ActiveCooldowns.Contains(Tag))
	{
		return -1.f;
	}

	return ActiveCooldowns[Tag].InitialTime;
}

bool UGameActionComponent::HasCooldown(FGameplayTag Tag) const
{
	return ActiveCooldowns.Contains(Tag);
}

void UGameActionComponent::CancelCooldown(FGameplayTag Tag)
{
	ActiveCooldowns.Remove(Tag);
}

void UGameActionComponent::GetCooldowns(TMap<FGameplayTag, FCooldownData>& OutCooldowns) const
{
	OutCooldowns.Append(ActiveCooldowns);
}

void UGameActionComponent::OnFinishedAction(const FGameActionHandle& Handle, bool bResult)
{
	TObjectPtr<UGameAction> Action;
	if (ActiveActions.RemoveAndCopyValue(Handle.Id, Action))
	{
		Action->HandleEndAction(bResult);

		UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) finished with '%s'"), *Action->GetName(), Handle.Id, bResult ? TEXT("success") : TEXT("failure"));

		OnActionFinished.Broadcast(this, Handle, Action, bResult);
	}
}

UGameActionComponent* UGameActionComponent::GetGameActionComponentFromActor(const AActor* Actor, bool bLookForComponent)
{
	if (!Actor)
	{
		return nullptr;
	}

	const IGameActionInterface* ActionInterface = Cast<IGameActionInterface>(Actor);
	if (ActionInterface)
	{
		return ActionInterface->GetGameActionComponent();
	}

	if (bLookForComponent)
	{
		// This is slow, give us a warning!
		UE_LOG(LogGameActions, Warning, TEXT("GetGameActionComponentFromActor called on %s when it does not implement IGameActionInterface"), *Actor->GetName());

		return Actor->FindComponentByClass<UGameActionComponent>();
	}

	return nullptr;
}

FAsyncCoroutine UGameActionComponent::InternalExecuteActionLatent(
	const FGameActionHandle& Handle,
	UGameAction* Action,
	EGameActionExecutionResult& Result)
{
	UGameActionComponent* ThisPtr = this;
	MA_WEAK_REF(Action, ThisPtr);

	bool bIsFinished = false;
	if (!Handle.IsValid())
	{
		co_return;
	}

	FDelegateHandle OnCancelledHandle;
	FDelegateHandle OnEndedHandle;

	MA_ON_LATENT_ABNORMAL_EXIT
	{
		if (Action)
		{
			Action->OnActionCancelled.Remove(OnCancelledHandle);
			Action->OnActionEnded.Remove(OnEndedHandle);
		}

		if (ThisPtr)
		{
			ThisPtr->CancelAction(Handle);
		}
	};

	OnCancelledHandle = Action->OnActionCancelled.AddLambda([&] (auto)
	{
		Result = EGameActionExecutionResult::Cancelled;
		bIsFinished = true;

		if (Action)
		{
			Action->OnActionCancelled.Remove(OnCancelledHandle);
			Action->OnActionEnded.Remove(OnEndedHandle);
		}
	});

	OnEndedHandle = Action->OnActionEnded.AddLambda([&] (auto, bool bResult)
	{
		Result = bResult ? EGameActionExecutionResult::Succeeded : EGameActionExecutionResult::Failed;
		bIsFinished = true;

		if (Action)
		{
			Action->OnActionCancelled.Remove(OnCancelledHandle);
			Action->OnActionEnded.Remove(OnEndedHandle);
		}
	});

	co_await MA_LATENT_UNTIL(bIsFinished);
}

UGameAction* UGameActionComponent::BeginExecuteAction(FGameplayTag Name, FGameActionHandle& OutHandle)
{
	const TSubclassOf<UGameAction>* ActionClass = AvailableActions.Find(Name);
	if (ActionClass == nullptr)
	{
		UE_LOG(LogGameActions, Warning, TEXT("Tried to execute action by unknown name '%s'"), *Name.ToString())
		return nullptr;
	}

	return BeginExecuteAction(*ActionClass, OutHandle);
}

UGameAction* UGameActionComponent::BeginExecuteAction(TSubclassOf<UGameAction> ActionClass, FGameActionHandle& OutHandle)
{
	if (IsBeingDestroyed() || !HasBegunPlay())
	{
		UE_LOG(LogGameActions, Log, TEXT("Ignoring BeginExecuteAction, %s is being destroyed or hasn't begun play"), *GetName());
		return nullptr;
	}

	if (!ActionClass)
	{
		return nullptr;
	}

	const UGameAction* DefaultAction = GetDefault<UGameAction>(ActionClass);
	if (DefaultAction->WithinActor)
	{
		if (!GetOwner() || !GetOwner()->IsA(DefaultAction->WithinActor))
		{
			UE_VLOG(GetOwner(), LogGameActions, Warning, TEXT("Action class '%s' refused execution (owner %s is not a %s)"), *ActionClass->GetName(), *GetNameSafe(GetOwner()), *GetNameSafe(DefaultAction->WithinActor));
			return nullptr;
		}
	}
	
	UGameAction* Action = NewObject<UGameAction>(this, *ActionClass);
	
	const FGameActionHandle Handle(NextActionId++);
	Action->BindHandle(Handle, this);

	if (!Action->CanExecuteAction())
	{
		UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) refused execution"), *Action->GetName(), Handle.Id);
		return nullptr;
	}

	FGameplayTagContainer RequiredTags = Action->RequireTags;

	// Check cooldown tags
	for (const auto& Cooldown : ActiveCooldowns)
	{
		if (Action->BlockTags.HasTag(Cooldown.Key))
		{
			UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) was blocked due to cooldown tag '%s'"), *Action->GetName(), Handle.Id, *Cooldown.Key.ToString());
			return nullptr;
		}

		RequiredTags.RemoveTag(Cooldown.Key);
	}
	
	// Check tags
	TArray<TPair<FGameActionHandle, UGameAction*>> ActionsToCancel;
	for (auto& Elem : ActiveActions)
	{
		UGameAction* OtherAction = Elem.Value;
		if (OtherAction->OwnedTags.HasAny(Action->CancelTags))
		{
			ActionsToCancel.Add(TPair<FGameActionHandle, UGameAction*>(FGameActionHandle(Elem.Key), OtherAction));
			continue;
		}

		if (OtherAction->OwnedTags.HasAny(Action->BlockTags) || Action->OwnedTags.HasAny(OtherAction->BlockTags))
		{
			UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) was blocked due to tags on action '%s' (h%u)"), *Action->GetName(), Handle.Id, *OtherAction->GetName(), Elem.Key);
			return nullptr;
		}

		RequiredTags.RemoveTags(OtherAction->OwnedTags);
	}

	if (RequiredTags.Num() != 0)
	{
		UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) was blocked due to not satisfying tag requirements"), *Action->GetName(), Handle.Id);
		return nullptr;
	}

	for (auto& ActionToCancel : ActionsToCancel)
	{
		UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) is cancelling action '%s' (h%u) due to tags"), *Action->GetName(), Handle.Id, *ActionToCancel.Value->GetName(), ActionToCancel.Key.Id);
		CancelActionInternal(ActionToCancel.Key, false);
	}

	OutHandle = Handle;

	ActiveActions.Add(Handle.Id, Action);
	Action->HandleBeginAction();

	UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) execution has begun"), *Action->GetName(), Handle.Id);

	OnActionActivated.Broadcast(this, Handle, Action);

	return Action;
}

int32 UGameActionComponent::CancelActionHandles(const TArray<FGameActionHandle>& Handles)
{
	for (const FGameActionHandle& Handle : Handles)
	{
		CancelAction(Handle);
	}

	return Handles.Num();
}

bool UGameActionComponent::CancelActionInternal(const FGameActionHandle& Handle, bool bIsManual)
{
	TObjectPtr<UGameAction> Action;
	if (!ActiveActions.RemoveAndCopyValue(Handle.Id, Action))
	{
		UE_LOG(LogGameActions, Warning, TEXT("Failed to cancel action: unknown handle id %d"), Handle.Id);
		return false;
	}

	Action->HandleCancelAction();
	OnActionCancelled.Broadcast(this, Handle, Action);

	if (bIsManual)
	{
		UE_VLOG(GetOwner(), LogGameActions, Verbose, TEXT("Action '%s' (h%u) was cancelled manually"), *Action->GetName(), Handle.Id);
	}
	
	return true;
}

void UGameActionComponent::TickCooldowns(float DeltaTime)
{
	TArray<FGameplayTag> ExpiredCooldowns;
	for (auto& Elem : ActiveCooldowns)
	{
		Elem.Value.TimeLeft -= DeltaTime;
		if (Elem.Value.TimeLeft <= 0.f)
		{
			ExpiredCooldowns.Add(Elem.Key);
		}
	}

	for (const FGameplayTag& Tag : ExpiredCooldowns)
	{
		ActiveCooldowns.Remove(Tag);
	}
}

void UGameActionComponent::TickActions(float DeltaTime)
{
	auto Actions = ActiveActions;
	for (auto& Elem : Actions)
	{
		UGameAction* Action = Elem.Value;
		if (Action && Action->bShouldTick)
		{
			TRACE_CPUPROFILER_EVENT_SCOPE_TEXT(*GetNameSafe(Action));
			Action->OnTickAction(DeltaTime);
		}
	}
}