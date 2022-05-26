// Copyright (c) MissiveArts LLC


#include "GameActionMontageCallbackProxy.h"

#include "GameAction.h"

UGameActionMontageCallbackProxy::UGameActionMontageCallbackProxy()
{
	MontageInstanceID = INDEX_NONE;
	bInterruptedCalledBeforeBlendingOut = false;
}

UGameActionMontageCallbackProxy* UGameActionMontageCallbackProxy::CreateProxyObjectForPlayMontage(UGameAction* InAction, USkeletalMeshComponent* InSkeletalMeshComponent, UAnimMontage* MontageToPlay,
                                                                                                  float PlayRate, float StartingPosition, FName StartingSection)
{
	UGameActionMontageCallbackProxy* Proxy = NewObject<UGameActionMontageCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->PlayMontage(InAction, InSkeletalMeshComponent, MontageToPlay, PlayRate, StartingPosition, StartingSection);
	return Proxy;
}

void UGameActionMontageCallbackProxy::BeginDestroy()
{
	UnbindDelegates();

	Super::BeginDestroy();
}

void UGameActionMontageCallbackProxy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		OnInterrupted.Broadcast(NAME_None);
		bInterruptedCalledBeforeBlendingOut = true;
	}
	else
	{
		OnBlendOut.Broadcast(NAME_None);
	}
}

void UGameActionMontageCallbackProxy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		OnCompleted.Broadcast(NAME_None);
	}
	else if (!bInterruptedCalledBeforeBlendingOut)
	{
		OnInterrupted.Broadcast(NAME_None);
	}

	if (Action)
	{
		Action->RemoveActiveMontage(Montage);
	}

	UnbindDelegates();
}

void UGameActionMontageCallbackProxy::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyBegin.Broadcast(NotifyName);
	}
}

void UGameActionMontageCallbackProxy::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyEnd.Broadcast(NotifyName);
	}
}

void UGameActionMontageCallbackProxy::OnActionCancelled(UGameAction* InAction)
{
	check(InAction && Action == InAction);
	UnbindDelegates();

	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		if (MontagePlaying)
		{
			AnimInstance->Montage_Stop(0, MontagePlaying);
		}
	}
}

void UGameActionMontageCallbackProxy::OnActionEnded(UGameAction* InAction, bool bResult)
{
	check(InAction && Action == InAction);
	UnbindDelegates();

	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		if (MontagePlaying)
		{
			AnimInstance->Montage_Stop(0, MontagePlaying);
		}
	}
}

bool UGameActionMontageCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}

void UGameActionMontageCallbackProxy::UnbindDelegates()
{
	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ThisClass::OnNotifyBeginReceived);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &ThisClass::OnNotifyEndReceived);
	}

	if (Action)
	{
		Action->OnActionEnded.RemoveAll(this);
		Action->OnActionCancelled.RemoveAll(this);
	}
}

void UGameActionMontageCallbackProxy::PlayMontage(UGameAction* InAction, USkeletalMeshComponent* InSkeletalMeshComponent, UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition,
	FName StartingSection)
{
	check(InAction);
	
	bool bPlayedSuccessfully = false;
	if (InAction && InSkeletalMeshComponent)
	{
		if (UAnimInstance* AnimInstance = InSkeletalMeshComponent->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition);
			bPlayedSuccessfully = (MontageLength > 0.f);

			if (bPlayedSuccessfully)
			{
				AnimInstancePtr = AnimInstance;
				if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
				{
					MontageInstanceID = MontageInstance->GetInstanceID();
				}

				if (StartingSection != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
				}

				BlendingOutDelegate.BindUObject(this, &ThisClass::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &ThisClass::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::OnNotifyBeginReceived);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ThisClass::OnNotifyEndReceived);

				Action = InAction;
				Action->OnActionEnded.AddUObject(this, &ThisClass::OnActionEnded);
				Action->OnActionCancelled.AddUObject(this, &ThisClass::OnActionCancelled);

				Action->AddActiveMontage(MontageToPlay);

				MontagePlaying = MontageToPlay;
			}
		}
	}

	if (!bPlayedSuccessfully)
	{
		OnInterrupted.Broadcast(NAME_None);
	}
}
