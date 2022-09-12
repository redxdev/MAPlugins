// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameActionComponent.h"
#include "UObject/NoExportTypes.h"
#include "GameActionTypes.h"
#include "GameplayTagContainer.h"
#include "GameAction.generated.h"

MAGAMEACTIONS_API DECLARE_LOG_CATEGORY_EXTERN(LogGameActions, Log, All);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGameActionEnded, class UGameAction*, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameActionCancelled, class UGameAction*);

UCLASS(Abstract, Blueprintable, Category = "Game Actions")
class MAGAMEACTIONS_API UGameAction : public UObject
{
	GENERATED_BODY()

    friend class UGameActionComponent;
    friend class FGameActionExecutionAction;

public:
    UGameAction();

    virtual UWorld* GetWorld() const override;

    // Tags owned by this action.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Action")
    FGameplayTagContainer OwnedTags;

    // Blocks actions with the given tags from being started.
    // Those actions will also prevent this action from starting unless they are
    // cancelled.
    // Does not block cooldown tags from being applied.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Action")
    FGameplayTagContainer BlockTags;

    // Cancels actions with the given tags when this action is first
    // started.
    // Does not affect cooldown tags.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Action")
    FGameplayTagContainer CancelTags;

    // Blocks execution of this action (but not others) if any tags are missing.
    // This is not predictive. For example, tags applied as the result of CancelTags stopping another action will not be taken into
    // consideration.
    // If you need a more complex query, consider overriding CanExecuteAction.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Action")
    FGameplayTagContainer RequireTags;

    // Cooldown tags to apply when this action completes.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Action")
    TMap<FGameplayTag, float> Cooldowns;

    // If true, cooldown tags are applied even when this action is cancelled.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Action")
    uint8 bApplyCooldownsOnCancel : 1;

    // If specified, this action can only be executed within the specified actor or a subclass.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Action")
    TSubclassOf<AActor> WithinActor;

    FOnGameActionCancelled OnActionCancelled;
    FOnGameActionEnded OnActionEnded;

    // Called before checking tags to give an opportunity to modify
    // our tags.
    UFUNCTION(BlueprintNativeEvent)
    bool CanExecuteAction();

    // Gets the actor that is calling this action.
    UFUNCTION(BlueprintCallable)
    AActor* GetInstigator() const;

    template<typename T>
    T* GetInstigator() const
    {
        return Cast<T>(GetInstigator());
    }

    UFUNCTION(BlueprintCallable)
    bool CancelAction();

    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsFinished() const
    {
        return bFinished;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool DidFinishSuccessfully() const
    {
        return bFinished && bFinishResult;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsCancelled() const
    {
        return bCancelled;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE bool IsPlayingMontage(UAnimSequenceBase* AnimSequence) const
    {
        return ActiveMontages.Contains(AnimSequence);
    }

    FORCEINLINE void AddActiveMontage(UAnimSequenceBase* AnimSequence)
    {
        ActiveMontages.Add(AnimSequence);
    }

    FORCEINLINE void RemoveActiveMontage(UAnimSequenceBase* AnimSequence)
    {
        ActiveMontages.Remove(AnimSequence);
    }

protected:
    UFUNCTION(BlueprintNativeEvent)
    void OnTickAction(float DeltaTime);

    UFUNCTION(BlueprintNativeEvent)
    void OnNotifyAction(FGameplayTag Tag, UObject* Notify);
    
    UFUNCTION(BlueprintNativeEvent)
    void OnBeginAction();

    // Called when explicitly cancelling an action. EndAction will not be called.
    UFUNCTION(BlueprintNativeEvent)
    void OnCancelAction();

    UFUNCTION(BlueprintNativeEvent)
    void OnEndAction(bool bResult);

    UFUNCTION(BlueprintCallable)
    FORCEINLINE FGameActionHandle GetActionHandle() const
    {
        return ActionHandle;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE class UGameActionComponent* GetGameActionComponent() const
    {
        return GameActionComponent;
    }

    UFUNCTION(BlueprintCallable)
    void FinishAction(bool bResult);

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game Action")
    uint8 bShouldTick:1;

private:
    FGameActionHandle ActionHandle;

    uint8 bFinished:1;
    uint8 bFinishResult:1;
    uint8 bCancelled:1;

    UPROPERTY()
    TObjectPtr<UGameActionComponent> GameActionComponent;
    
    void BindHandle(const FGameActionHandle& Handle, UGameActionComponent* Component);
    void Cleanup();

    void HandleBeginAction();
    void HandleCancelAction();
    void HandleEndAction(bool bResult);

    void ApplyCooldowns();

    UPROPERTY()
    TSet<TObjectPtr<UAnimSequenceBase>> ActiveMontages;
};