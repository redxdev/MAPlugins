// Copyright (c) MissiveArts LLC


#include "AnimNotify_GameAction.h"

#include "GameAction.h"
#include "GameActionComponent.h"

static void NotifyGameActions(UObject* Notify, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, FGameplayTag Tag)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (UGameActionComponent* Comp = UGameActionComponent::GetGameActionComponentFromActor(MeshComp->GetOwner(), true))
		{
			Comp->NotifyActiveActionsFromMontage(Tag, Notify, Animation);
		}
	}
}

FString UAnimNotify_GameAction::GetNotifyName_Implementation() const
{
	return Tag.ToString();
}

void UAnimNotify_GameAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	NotifyGameActions(this, MeshComp, Animation, Tag);
}

FString UAnimNotifyState_GameAction::GetNotifyName_Implementation() const
{
	return BeginTag.IsValid() ? BeginTag.ToString() : EndTag.ToString();
}

void UAnimNotifyState_GameAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	NotifyGameActions(this, MeshComp, Animation, BeginTag);
}

void UAnimNotifyState_GameAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	NotifyGameActions(this, MeshComp, Animation, EndTag);
}

static void ForEachGameActionRunningMontage(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, TFunctionRef<void(UGameAction* Action)> Func)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (UGameActionComponent* Comp = UGameActionComponent::GetGameActionComponentFromActor(MeshComp->GetOwner(), true))
		{
			TArray<FGameActionHandle> Handles;
			Comp->GetActiveActionHandles(Handles);
			for (const FGameActionHandle& Handle : Handles)
			{
				if (UGameAction* Action = Comp->GetActiveAction(Handle))
				{
					if (Action->IsPlayingMontage(Animation))
					{
						Func(Action);
					}
				}
			}
		}
	}
}

void UAnimNotify_OwningGameAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ForEachGameActionRunningMontage(
		MeshComp,
		Animation,
		[this, MeshComp, Animation, EventReference] (UGameAction* Action)
		{
			Received_ActionNotify(Action, MeshComp, Animation, EventReference);
		});
}

void UAnimNotifyState_OwningGameAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ForEachGameActionRunningMontage(
		MeshComp,
		Animation,
		[this, MeshComp, Animation, TotalDuration, EventReference] (UGameAction* Action)
		{
			Received_ActionNotifyBegin(Action, MeshComp, Animation, TotalDuration, EventReference);
		});
}

void UAnimNotifyState_OwningGameAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ForEachGameActionRunningMontage(
		MeshComp,
		Animation,
		[this, MeshComp, Animation, EventReference] (UGameAction* Action)
		{
			Received_ActionNotifyEnd(Action, MeshComp, Animation, EventReference);
		});
}
