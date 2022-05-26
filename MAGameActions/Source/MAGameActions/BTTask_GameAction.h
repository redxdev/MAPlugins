// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameAction.h"
#include "BTTask_GameAction.generated.h"

struct FBTTask_GameAction_Memory
{
	TWeakObjectPtr<UGameAction> Action;
	TWeakObjectPtr<class UBTTask_GameAction> Task;
	TWeakObjectPtr<class UBehaviorTreeComponent> BTComp;
	
	void Bind(UGameAction* InAction, class UBTTask_GameAction* InTask, UBehaviorTreeComponent& InBTComp);
	void Unbind();

private:
	void OnActionEnded(UGameAction* InAction, bool bResult);
	void OnActionCancelled(UGameAction* InAction);
};

UCLASS()
class MAGAMEACTIONS_API UBTTask_GameAction : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_GameAction();

	UPROPERTY(EditAnywhere, Category = "Game Action", meta = (Categories = "ActionId"))
	FGameplayTag ActionName;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
