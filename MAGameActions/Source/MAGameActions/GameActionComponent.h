// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameActionTypes.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LatentActions.h"
#include "UE5Coro/AsyncCoroutine.h"
#include "GameActionComponent.generated.h"

class UGameAction;

UENUM()
enum class EGameActionExecutionResult : uint8
{
	Succeeded,
	Failed,
	Cancelled
};

struct FCooldownData
{
	float InitialTime;
	float TimeLeft;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGameActionActivationDelegate, class UGameActionComponent*, Component, const FGameActionHandle&, Handle, class UGameAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGameActionCanceledDelegate, class UGameActionComponent*, Component, const FGameActionHandle&, Handle, class UGameAction*, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGameActionFinishedDelegate, class UGameActionComponent*, Component, const FGameActionHandle&, Handle, class UGameAction*, Action, bool, bSuccess);

UCLASS( ClassGroup=(Custom), Category = "Game Actions", meta=(BlueprintSpawnableComponent) )
class MAGAMEACTIONS_API UGameActionComponent : public UActorComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:	
	UGameActionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "ActionId", ForceInlineRow))
	TMap<FGameplayTag, TSubclassOf<UGameAction>> AvailableActions;

	// These actions will execute on BeginPlay, provided they aren't blocked for some reason.
	// They are applied in the order given here.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameAction>> InitialActions;

	UPROPERTY(BlueprintAssignable)
	FGameActionActivationDelegate OnActionActivated;

	UPROPERTY(BlueprintAssignable)
	FGameActionFinishedDelegate OnActionFinished;

	UPROPERTY(BlueprintAssignable)
	FGameActionCanceledDelegate OnActionCancelled;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// -- Begin IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	// This doesn't _need_ to be reimplemented, but it's allows us to (sometimes) be faster than building the full list of tags.
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	// -- End IGameplayTagAssetInterface

	UFUNCTION(BlueprintCallable)
	bool IsActionActive(const FGameActionHandle& Handle) const;

	UFUNCTION(BlueprintCallable)
	UGameAction* GetActiveAction(const FGameActionHandle& Handle) const;

	UFUNCTION(BlueprintCallable)
	void GetActiveActionHandles(TArray<FGameActionHandle>& ActionHandles) const;

	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = "ActionClass"))
	UGameAction* GetActiveActionByClass(TSubclassOf<UGameAction> ActionClass) const;

	UFUNCTION(BlueprintCallable)
	void GetActiveActionHandlesByClass(TSubclassOf<UGameAction> ActionClass, TArray<FGameActionHandle>& ActionHandles) const;

	UGameAction* ExecuteAction(UPARAM(meta = (Categories = "ActionId")) FGameplayTag Name);

	UFUNCTION(BlueprintCallable)
	UGameAction* ExecuteAction(UPARAM(meta = (Categories = "ActionId")) FGameplayTag Name, FGameActionHandle& OutHandle);

	UGameAction* ExecuteActionByClass(TSubclassOf<UGameAction> ActionClass);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Action (by class)", DeterminesOutputType="ActionClass"))
	UGameAction* ExecuteActionByClass(TSubclassOf<UGameAction> ActionClass, FGameActionHandle& OutHandle);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Action and Wait", Latent, LatentInfo = LatentInfo, ExpandEnumAsExecs = "ActionResult"))
	FAsyncCoroutine ExecuteActionLatent(UPARAM(meta = (Categories = "ActionId")) FGameplayTag Name, struct FLatentActionInfo LatentInfo, EGameActionExecutionResult& ActionResult);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Action and Wait (by class)", Latent, LatentInfo = LatentInfo, ExpandEnumAsExecs = "ActionResult"))
	FAsyncCoroutine ExecuteActionByClassLatent(TSubclassOf<UGameAction> ActionClass, struct FLatentActionInfo LatentInfo, EGameActionExecutionResult& ActionResult);

	UFUNCTION(BlueprintCallable)
	bool CancelAction(const FGameActionHandle& Handle);

	UFUNCTION(BlueprintCallable)
	int32 CancelActionsByTag(const FGameplayTagQuery Query);

	UFUNCTION(BlueprintCallable)
	int32 CancelActionsByClass(TSubclassOf<UGameAction> ActionClass);

	UFUNCTION(BlueprintCallable)
	int32 CancelAllActions();

	// "Tag" here refers to owned tags, not the "name" of the action as configured on this component.
	UFUNCTION(BlueprintCallable)
	void NotifyActiveActions(FGameplayTag Tag, UObject* Notify = nullptr, TSubclassOf<UGameAction> ActionClass = nullptr);

	void NotifyActiveActionsFromMontage(FGameplayTag Tag, UObject* Notify = nullptr, class UAnimSequenceBase* Montage = nullptr);

	UFUNCTION(BlueprintCallable)
	bool FindActionsByTag(FGameplayTagQuery Query, TArray<FGameActionHandle>& OutHandles) const;

	UFUNCTION(BlueprintCallable)
	bool ContainsActionWithTag(FGameplayTagQuery Query) const;

	UFUNCTION(BlueprintCallable)
	bool FindActionsByClass(TSubclassOf<UGameAction> ActionClass, TArray<FGameActionHandle>& OutHandles) const;

	UFUNCTION(BlueprintCallable)
	bool ContainsActionOfClass(TSubclassOf<UGameAction> ActionClass) const;

	// Creates a cooldown tag that will be removed after the given time. This can block
	// new actions from executing, but will not affect existing actions.
	UFUNCTION(BlueprintCallable)
	void ApplyCooldown(FGameplayTag Tag, float CooldownTime);

	UFUNCTION(BlueprintCallable)
	float GetCooldownTimeLeft(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	float GetCooldownInitialTime(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	bool HasCooldown(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable)
	void CancelCooldown(FGameplayTag Tag);

	void GetCooldowns(TMap<FGameplayTag, FCooldownData>& OutCooldowns) const;

	// Meant to be called by UGameAction only
	void OnFinishedAction(const FGameActionHandle& Handle, bool bResult);

	UFUNCTION(BlueprintPure, meta = (DefaultToSelf = Actor))
	static UGameActionComponent* GetGameActionComponentFromActor(const AActor* Actor, bool bLookForComponent = false);

protected:
	FORCEINLINE const TMap<uint32, TObjectPtr<UGameAction>> GetActiveActions() const { return ActiveActions; }

private:
	UPROPERTY()
	TMap<uint32, TObjectPtr<UGameAction>> ActiveActions;

	TMap<FGameplayTag, FCooldownData> ActiveCooldowns;

	uint32 NextActionId;
	
	FAsyncCoroutine InternalExecuteActionLatent(const FGameActionHandle& Handle, UGameAction* Action, EGameActionExecutionResult& Result, FLatentActionInfo LatentInfo);

	UGameAction* BeginExecuteAction(FGameplayTag Name, FGameActionHandle& OutHandle);
	UGameAction* BeginExecuteAction(TSubclassOf<UGameAction> ActionClass, FGameActionHandle& OutHandle);

	int32 CancelActionHandles(const TArray<FGameActionHandle>& Handles);
	bool CancelActionInternal(const FGameActionHandle& Handle, bool bIsManual);

	void TickCooldowns(float DeltaTime);
	void TickActions(float DeltaTime);
};