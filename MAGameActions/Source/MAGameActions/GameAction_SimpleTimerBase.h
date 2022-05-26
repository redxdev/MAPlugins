// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameAction.h"
#include "GameAction_SimpleTimerBase.generated.h"

/**
 * Action that removes itself after a specified amount of time.
 */
UCLASS(Abstract)
class MAGAMEACTIONS_API UGameAction_SimpleTimerBase : public UGameAction
{
	GENERATED_BODY()

public:
	UGameAction_SimpleTimerBase();
	
	// UGameAction Interface
	virtual void OnBeginAction_Implementation() override;
	virtual void OnEndAction_Implementation(bool bResult) override;
	virtual void OnCancelAction_Implementation() override;
	// End UGameAction Interface
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Timer Action")
	float TimeBeforeCompletion = 0.0f;

private:
	FTimerHandle TimerHandle;
};
