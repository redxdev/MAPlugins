// Copyright (c) MissiveArts LLC


#include "GameAction.h"
#include "GameActionComponent.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogGameActions);

UGameAction::UGameAction()
{
    ActionHandle = FGameActionHandle::Invalid;
    bShouldTick = true;
    bApplyCooldownsOnCancel = true;
}

UWorld* UGameAction::GetWorld() const
{
    if (!GameActionComponent)
    {
        return nullptr;
    }
    
    return GameActionComponent->GetWorld();
}

bool UGameAction::CanExecuteAction_Implementation()
{
    return true;
}

void UGameAction::HandleBeginAction()
{
    OnBeginAction();
}

void UGameAction::HandleCancelAction()
{
    Cleanup();
    bCancelled = true;
    OnCancelAction();
    OnActionCancelled.Broadcast(this);

    if (bApplyCooldownsOnCancel)
    {
        ApplyCooldowns();
    }
}

void UGameAction::HandleEndAction(bool bResult)
{
    Cleanup();

    bFinished = true;
    bFinishResult = bResult;
    
    OnEndAction(bResult);
    OnActionEnded.Broadcast(this, bResult);

    ApplyCooldowns();
}

void UGameAction::ApplyCooldowns()
{
    for (const auto& Cooldown : Cooldowns)
    {
        GameActionComponent->ApplyCooldown(Cooldown.Key, Cooldown.Value);
    }
}

AActor* UGameAction::GetInstigator() const
{
    if (!GameActionComponent)
    {
        return nullptr;
    }

    return GameActionComponent->GetOwner();
}

bool UGameAction::CancelAction()
{
    if (!GameActionComponent || ActionHandle == FGameActionHandle::Invalid)
    {
        return false;
    }

    return GameActionComponent->CancelAction(ActionHandle);
}

void UGameAction::OnNotifyAction_Implementation(FGameplayTag Tag, UObject* Notify)
{
}

void UGameAction::OnTickAction_Implementation(float DeltaTime)
{
}

void UGameAction::OnBeginAction_Implementation()
{
}

void UGameAction::OnCancelAction_Implementation()
{
}

void UGameAction::OnEndAction_Implementation(bool bResult)
{
}

void UGameAction::FinishAction(bool bResult)
{
    check(GetGameActionComponent());
    GetGameActionComponent()->OnFinishedAction(GetActionHandle(), bResult);
}

void UGameAction::BindHandle(const FGameActionHandle& Handle, UGameActionComponent* Component)
{
    ActionHandle = Handle;
    GameActionComponent = Component;
}

void UGameAction::Cleanup()
{
    check(GetWorld());

    FLatentActionManager& LatentActionManager = GetWorld()->GetLatentActionManager();
    LatentActionManager.RemoveActionsForObject(this);
}
