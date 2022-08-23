// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "Damage.generated.h"

USTRUCT(BlueprintType)
struct MADAMAGE_API FMADamageResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> Target = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> DamageCauser = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AController> Instigator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Damage"))
	FGameplayTagContainer IncomingTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "DamageResponse"))
	FGameplayTagContainer ResponseTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult Hit;

	void GetBestHitInfo(FHitResult& OutHitInfo, FVector& OutImpulseDir) const;

	bool IsApplyingDamage() const;
	bool HasHit() const;
};

// Base class for damage types. Doesn't do anything by itself.
UCLASS(Blueprintable)
class MADAMAGE_API UMADamageTypeBase : public UObject
{
	GENERATED_BODY()

public:
	// Called before letting an actor handle damage calculations.
	UFUNCTION(BlueprintNativeEvent)
	void OnPreApplyDamage(UPARAM(ref) FMADamageResponse& Event) const;
	virtual void OnPreApplyDamage_Implementation(FMADamageResponse& Event) const;
};

// Implements a simple tag-based damage type.
UCLASS(Blueprintable)
class MADAMAGE_API UMATaggedDamageType : public UMADamageTypeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Damage"))
	FGameplayTagContainer Tags;

	// This applies all of this type's damage tags to FMADamageResponse::IncomingTags
	virtual void OnPreApplyDamage_Implementation(FMADamageResponse& Event) const override;
};

UCLASS()
class MADAMAGE_API UMADamageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintPure, Category = "Game|Damage")
	static bool DidApplyDamage(const FMADamageResponse& Event)
	{
		return Event.IsApplyingDamage();
	}

	UFUNCTION(BlueprintPure, Category = "Game|Damage")
	static bool HasHit(const FMADamageResponse& Event)
	{
		return Event.HasHit();
	}

	UFUNCTION(BlueprintPure, Category = "Game|Damage")
	static void GetBestHitInfo(const FMADamageResponse& Event, FHitResult& OutHitInfo, FVector& OutImpulseDir);

	UFUNCTION(BlueprintCallable, Category = "Game|Damage", meta = (DisplayName = "Apply Damage (MADamage)"))
	static bool ApplyDamage(AActor* Target, float Damage, TSubclassOf<UMADamageTypeBase> DamageType, AActor* DamageCauser,
	                        AController* Instigator, FGameplayTagContainer AdditionalTags, FMADamageResponse& OutResponse);

	UFUNCTION(BlueprintCallable, Category = "Game|Damage", meta = (DisplayName = "Apply Damage from Hit Result (MADamage)"))
	static bool ApplyDamageFromHit(AActor* Target, float Damage, TSubclassOf<UMADamageTypeBase> DamageType, AActor* DamageCauser,
	                               AController* Instigator, FGameplayTagContainer AdditionalTags, const FHitResult& Hit, FMADamageResponse& OutResponse);

	// Bit of a hack, called before attempting to apply damage or event constructing a damage response.
	// Meant as a hook to check if an actor is even remotely able to be damaged from a global system perspective.
	// i.e. does the actor have a "damageable" tag?
	DECLARE_DELEGATE_RetVal_OneParam(bool, FCanApplyDamageToActorDelegate, AActor*);
	static FCanApplyDamageToActorDelegate CanApplyDamageToActor;

private:
	static bool InternalApplyDamage(AActor* Target, float Damage, TSubclassOf<UMADamageTypeBase> DamageType, AActor* DamageCauser,
	                                AController* Instigator, const FGameplayTagContainer& AdditionalTags, const FHitResult& Hit, FMADamageResponse& OutResponse);
};

UINTERFACE(BlueprintType)
class UMADamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class MADAMAGE_API IMADamageableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool OnIncomingDamage(UPARAM(ref) FMADamageResponse& Event);
};
