// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Animation/AnimInstance.h"
#include "GameActionMontageCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameActionMontagePlayDelegate, FName, NotifyName);

// This is largely copied from UPlayMontageCallbackProxy but with the ability to be cancelled by a
// game action ending.
UCLASS(MinimalAPI)
class UGameActionMontageCallbackProxy : public UObject
{
	GENERATED_BODY()

public:
	UGameActionMontageCallbackProxy();

	// Called when Montage finished playing and wasn't interrupted
	UPROPERTY(BlueprintAssignable)
	FOnGameActionMontagePlayDelegate OnCompleted;

	// Called when Montage starts blending out and is not interrupted
	UPROPERTY(BlueprintAssignable)
	FOnGameActionMontagePlayDelegate OnBlendOut;

	// Called when Montage has been interrupted (or failed to play)
	UPROPERTY(BlueprintAssignable)
	FOnGameActionMontagePlayDelegate OnInterrupted;
	
	UPROPERTY(BlueprintAssignable)
	FOnGameActionMontagePlayDelegate OnNotifyBegin;

	UPROPERTY(BlueprintAssignable)
	FOnGameActionMontagePlayDelegate OnNotifyEnd;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DefaultToSelf = "InAction"))
	static MAGAMEACTIONS_API UGameActionMontageCallbackProxy* CreateProxyObjectForPlayMontage(
		class UGameAction* InAction,
		class USkeletalMeshComponent* InSkeletalMeshComponent,
		class UAnimMontage* MontageToPlay,
		float PlayRate = 1.f,
		float StartingPosition = 0.f,
		FName StartingSection = NAME_None);

	virtual void BeginDestroy() override;

	FORCEINLINE bool IsPlaying() const
	{
		return MontagePlaying && AnimInstancePtr.IsValid() && AnimInstancePtr->Montage_IsPlaying(MontagePlaying);
	}

	FORCEINLINE UAnimInstance* GetAnimInstance() const
	{
		return AnimInstancePtr.Get();
	}

protected:
	UFUNCTION()
    void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
    void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
    void OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnActionCancelled(class UGameAction* InAction);

	UFUNCTION()
	void OnActionEnded(class UGameAction* InAction, bool bResult);

private:
	
	UPROPERTY()
	TObjectPtr<class UGameAction> Action;

	UPROPERTY()
	TObjectPtr<class UAnimMontage> MontagePlaying;

	TWeakObjectPtr<UAnimInstance> AnimInstancePtr;
	int32 MontageInstanceID;
	uint32 bInterruptedCalledBeforeBlendingOut : 1;

	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;
	void UnbindDelegates();

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

	void PlayMontage(
		class UGameAction* InAction,
        class USkeletalMeshComponent* InSkeletalMeshComponent,
        class UAnimMontage* MontageToPlay,
        float PlayRate = 1.f,
        float StartingPosition = 0.f,
        FName StartingSection = NAME_None);
};
