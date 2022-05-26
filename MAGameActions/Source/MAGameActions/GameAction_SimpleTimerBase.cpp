// Copyright (c) MissiveArts LLC


#include "GameAction_SimpleTimerBase.h"

UGameAction_SimpleTimerBase::UGameAction_SimpleTimerBase()
{
	bShouldTick = false;
}

void UGameAction_SimpleTimerBase::OnBeginAction_Implementation()
{
	Super::OnBeginAction_Implementation();

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(
		TimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this] { CancelAction(); }),
		TimeBeforeCompletion,
		false);
}

void UGameAction_SimpleTimerBase::OnEndAction_Implementation(bool bResult)
{
	Super::OnEndAction_Implementation(bResult);

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(TimerHandle);
}

void UGameAction_SimpleTimerBase::OnCancelAction_Implementation()
{
	Super::OnCancelAction_Implementation();
	
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(TimerHandle);
}
