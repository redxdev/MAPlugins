// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotify_GameAction.generated.h"

// A notify that sends a tag to the game action(s) that are running the current montage.
UCLASS(DisplayName = "Notify Game Action")
class MAGAMEACTIONS_API UAnimNotify_GameAction : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Notify"))
	FGameplayTag Tag;

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// A notify that sends a tag to the game action(s) that are running the current montage.
UCLASS(DisplayName = "Notify Game Action (State)")
class MAGAMEACTIONS_API UAnimNotifyState_GameAction : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Notify"))
	FGameplayTag BeginTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Notify"))
	FGameplayTag EndTag;

	virtual FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// A notify that runs for any game action that owns the montage that is triggering this notify.
UCLASS(Abstract)
class MAGAMEACTIONS_API UAnimNotify_OwningGameAction : public UAnimNotify
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ActionNotify"))
	void Received_ActionNotify(class UGameAction* Action, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

// A notify that runs for any game action that owns the montage that is triggering this notify.
UCLASS(Abstract)
class MAGAMEACTIONS_API UAnimNotifyState_OwningGameAction : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ActionNotifyBegin"))
	void Received_ActionNotifyBegin(class UGameAction* Action, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ActionNotifyEnd"))
	void Received_ActionNotifyEnd(class UGameAction* Action, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};