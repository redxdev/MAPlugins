// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/ActorComponent.h"
#include "AIStateComponent.generated.h"

USTRUCT(BlueprintType)
struct MAGAMEPLAY_API FAIState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;
};

// Overrides and extends states in UAIStateComponent.
UCLASS(MinimalAPI, ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class UPawnAIStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Applied when a controller possesses us, if no other state is active.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI State", meta = (Categories = "AI.State"))
	FGameplayTag DefaultState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI State", meta = (Categories = "AI.State", ForceInlineRow))
	TMap<FGameplayTag, FAIState> States;

	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool ApplyState(UPARAM(meta = (Categories = "AI.State")) FGameplayTag StateTag);

	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool CancelState();

	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool ReapplyActiveState();

private:
	class UAIStateComponent* GetAIStateComponent() const;
};

UCLASS(MinimalAPI, ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class UAIStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIStateComponent();
	
	// Applied when the controller possesses a pawn, if no other state is active.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI State", meta = (Categories = "AI.State"))
	FGameplayTag DefaultState;

	// Sets the active state to None when the controller unpossesses the pawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI State", AdvancedDisplay)
	uint8 bResetStateWhenUnpossessed:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI State", meta = (Categories = "AI.State", ForceInlineRow))
	TMap<FGameplayTag, FAIState> States;

	// Returns true when the state has been successfully applied.
	// On failure, the current state is set to None.
	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool ApplyState(UPARAM(meta = (Categories = "AI.State")) FGameplayTag StateTag);

	// Sets the active state to none.
	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool CancelState();

	// Utility function to reapply the current state.
	// Useful if something else changes the active AI behavior.
	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool ReapplyActiveState();

	UFUNCTION(BlueprintCallable, Category = AI)
	FORCEINLINE FGameplayTag GetCurrentStateTag() const { return CurrentState; }

	virtual void PostInitProperties() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool FindState(FGameplayTag StateTag, FAIState& OutState);
	void InternalApplyState(FGameplayTag StateTag, const FAIState& State);

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	void ResolvePawnAIState(APawn* NewPawn);

	UPROPERTY(VisibleAnywhere, Category = "AI State")
	FGameplayTag CurrentState;

	TWeakObjectPtr<UPawnAIStateComponent> PawnAIState;
};