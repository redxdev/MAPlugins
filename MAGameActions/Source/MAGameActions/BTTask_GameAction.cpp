// Copyright (c) MissiveArts LLC

#include "BTTask_GameAction.h"
#include "AIController.h"
#include "GameAction.h"
#include "GameActionComponent.h"

static UGameActionComponent* GetGameActionComponent(UBehaviorTreeComponent& OwnerComp)
{
	if (AAIController* Controller = OwnerComp.GetAIOwner())
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			if (UGameActionComponent* Comp = UGameActionComponent::GetGameActionComponentFromActor(Pawn))
			{
				return Comp;
			}
		}

		return UGameActionComponent::GetGameActionComponentFromActor(Controller);
	}

	return nullptr;
}

void FBTTask_GameAction_Memory::Bind(UGameAction* InAction, UBTTask_GameAction* InTask, UBehaviorTreeComponent& InBTComp)
{
	check(InAction);
	check(InTask);

	Action = InAction;
	Task = InTask;
	BTComp = &InBTComp;

	InAction->OnActionEnded.AddRaw(this, &FBTTask_GameAction_Memory::OnActionEnded);
	InAction->OnActionCancelled.AddRaw(this, &FBTTask_GameAction_Memory::OnActionCancelled);
}

void FBTTask_GameAction_Memory::Unbind()
{
	if (UGameAction* ActionPtr = Action.Get())
	{
		ActionPtr->OnActionEnded.RemoveAll(this);
		ActionPtr->OnActionCancelled.RemoveAll(this);
	}

	Action.Reset();
	Task.Reset();
	BTComp.Reset();
}

void FBTTask_GameAction_Memory::OnActionEnded(UGameAction* InAction, bool bResult)
{
	if (InAction != Action.Get())
	{
		return;
	}

	if (UBTTask_GameAction* TaskPtr = Task.Get())
	{
		TaskPtr->FinishLatentTask(*BTComp.Get(), bResult ? EBTNodeResult::Succeeded : EBTNodeResult::Failed);
	}

	Unbind();
}

void FBTTask_GameAction_Memory::OnActionCancelled(UGameAction* InAction)
{
	if (InAction != Action.Get())
	{
		return;
	}
	
	if (UBTTask_GameAction* TaskPtr = Task.Get())
	{
		TaskPtr->FinishLatentTask(*BTComp.Get(), EBTNodeResult::Failed);
	}

	Unbind();
}

UBTTask_GameAction::UBTTask_GameAction()
{
	NodeName = "Execute Game Action";
}

EBTNodeResult::Type UBTTask_GameAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTTask_GameAction_Memory* MyMemory = CastInstanceNodeMemory<FBTTask_GameAction_Memory>(NodeMemory);
	UGameActionComponent* GameActionComponent = GetGameActionComponent(OwnerComp);
	if (!GameActionComponent)
	{
		return EBTNodeResult::Failed;
	}

	UGameAction* Action = GameActionComponent->ExecuteAction(ActionName);
	if (!Action)
	{
		return EBTNodeResult::Failed;
	}

	if (Action->IsCancelled())
	{
		return EBTNodeResult::Failed;
	}

	if (Action->IsFinished())
	{
		return Action->DidFinishSuccessfully() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	MyMemory->Bind(Action, this, OwnerComp);

	return EBTNodeResult::InProgress;
}

uint16 UBTTask_GameAction::GetInstanceMemorySize() const
{
	return sizeof(FBTTask_GameAction_Memory);
}

void UBTTask_GameAction::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	FBTTask_GameAction_Memory* MyMemory = CastInstanceNodeMemory<FBTTask_GameAction_Memory>(NodeMemory);
	if (InitType == EBTMemoryInit::Initialize)
	{
		new (MyMemory) FBTTask_GameAction_Memory;
	}
}

void UBTTask_GameAction::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	FBTTask_GameAction_Memory* MyMemory = CastInstanceNodeMemory<FBTTask_GameAction_Memory>(NodeMemory);
	if (CleanupType == EBTMemoryClear::Destroy)
	{
		MyMemory->Unbind();
	}
}

FString UBTTask_GameAction::GetStaticDescription() const
{
	return FString::Printf(TEXT("Action Name: %s"), *ActionName.ToString());
}

EBTNodeResult::Type UBTTask_GameAction::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTTask_GameAction_Memory* MyMemory = CastInstanceNodeMemory<FBTTask_GameAction_Memory>(NodeMemory);
	if (UGameAction* Action = MyMemory->Action.Get())
	{
		Action->CancelAction();
	}

	MyMemory->Unbind();
	return EBTNodeResult::Aborted;
}