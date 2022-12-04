// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "StateTreeReference.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/ActorComponent.h"
#include "AIStateComponent.generated.h"

UENUM()
enum class EAIStateMode : uint8
{
	BehaviorTree,
	StateTree
};

USTRUCT(BlueprintType)
struct MAGAMEPLAY_API FAIState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIStateMode Mode = EAIStateMode::BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "Mode == EAIStateMode::BehaviorTree", EditConditionHides))
	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;

	// No blueprint support (yet)
	UPROPERTY(EditAnywhere, meta=(Schema="/Script/GameplayStateTreeModule.StateTreeComponentSchema", EditCondition = "Mode == EAIStateMode::StateTree", EditConditionHides))
	FStateTreeReference StateTree;

	bool IsValid() const;
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
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool ApplyState(UPARAM(meta = (Categories = "AI.State")) FGameplayTag StateTag);

	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool CancelState();

	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API bool ReapplyActiveState();

	UFUNCTION(BlueprintCallable, Category = AI)
	MAGAMEPLAY_API FGameplayTag GetCurrentStateTag() const;

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
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool FindState(FGameplayTag StateTag, FAIState& OutState);
	bool InternalApplyState(FGameplayTag StateTag, const FAIState& State);

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	UFUNCTION()
	void OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	void ResolvePawnAIState(APawn* NewPawn);

	UPROPERTY(VisibleAnywhere, Category = "AI State")
	FGameplayTag CurrentState;

	TWeakObjectPtr<UPawnAIStateComponent> PawnAIState;
};