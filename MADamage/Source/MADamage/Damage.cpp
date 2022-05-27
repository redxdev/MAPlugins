// Copyright (c) MissiveArts LLC

#include "Damage.h"
#include "NativeGameplayTags.h"

// Counts as having applied damage even if the damage value is <= 0
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_DamageResponse_ApplyWithoutDamage, "DamageResponse.ApplyWithoutDamage");

// Damage comes with an FHitResult
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Damage_FromHit, "Damage.FromHit");

void FMADamageResponse::GetBestHitInfo(FHitResult& OutHitInfo, FVector& OutImpulseDir) const
{
    if (HasHit())
    {
        OutHitInfo = Hit;
        OutImpulseDir = -Hit.ImpactNormal;
    }
    else if (Target)
    {
        // fill out the hitinfo as best we can
        OutHitInfo.HitObjectHandle = FActorInstanceHandle(Target);
        OutHitInfo.bBlockingHit = true;
        OutHitInfo.BoneName = NAME_None;
        OutHitInfo.Component = Cast<UPrimitiveComponent>(Target->GetRootComponent());
		
        // assume the actor got hit in the center of his root component
        OutHitInfo.ImpactPoint = Target->GetActorLocation();
        OutHitInfo.Location = OutHitInfo.ImpactPoint;
		
        // assume hit came from instigator's location
        OutImpulseDir = DamageCauser ? 
            ( OutHitInfo.ImpactPoint - DamageCauser->GetActorLocation() ).GetSafeNormal()
            : FVector::ZeroVector;

        // assume normal points back toward instigator
        OutHitInfo.ImpactNormal = -OutImpulseDir;
        OutHitInfo.Normal = OutHitInfo.ImpactNormal;
    }
}

bool FMADamageResponse::IsApplyingDamage() const
{
    return Damage > 0.f || ResponseTags.HasTag(TAG_DamageResponse_ApplyWithoutDamage);
}

bool FMADamageResponse::HasHit() const
{
    return IncomingTags.HasTag(TAG_Damage_FromHit);
}

void UMADamageTypeBase::OnPreApplyDamage_Implementation(FMADamageResponse& Event) const
{
}

void UMATaggedDamageType::OnPreApplyDamage_Implementation(FMADamageResponse& Event) const
{
    Super::OnPreApplyDamage_Implementation(Event);
    Event.IncomingTags.AppendTags(Tags);
}

void UMADamageLibrary::GetBestHitInfo(const FMADamageResponse& Event, FHitResult& OutHitInfo, FVector& OutImpulseDir)
{
    return Event.GetBestHitInfo(OutHitInfo, OutImpulseDir);
}

bool UMADamageLibrary::ApplyDamage(AActor* Target, float Damage, TSubclassOf<UMADamageTypeBase> DamageType, AActor* DamageCauser,
                                      AController* Instigator, FGameplayTagContainer AdditionalTags, FMADamageResponse& OutResponse)
{
    if (!Target)
    {
        return false;
    }

    return InternalApplyDamage(Target, Damage, DamageType, DamageCauser, Instigator, AdditionalTags, FHitResult(), OutResponse);
}

bool UMADamageLibrary::ApplyDamageFromHit(AActor* Target, float Damage, TSubclassOf<UMADamageTypeBase> DamageType,
                                             AActor* DamageCauser, AController* Instigator, FGameplayTagContainer AdditionalTags, const FHitResult& Hit, FMADamageResponse& OutResponse)
{
    if (!Target)
    {
        return false;
    }

    AdditionalTags.AddTag(TAG_Damage_FromHit);

    return InternalApplyDamage(Target, Damage, DamageType, DamageCauser, Instigator, AdditionalTags, Hit, OutResponse);
}

UMADamageLibrary::FCanApplyDamageToActorDelegate UMADamageLibrary::CanApplyDamageToActor = UMADamageLibrary::FCanApplyDamageToActorDelegate();

bool UMADamageLibrary::InternalApplyDamage(AActor* Target, float Damage, TSubclassOf<UMADamageTypeBase> DamageType,
                                              AActor* DamageCauser, AController* Instigator, const FGameplayTagContainer& AdditionalTags, const FHitResult& Hit, FMADamageResponse& OutResponse)
{
    if (!IsValid(Target))
    {
        return false;
    }

    if (CanApplyDamageToActor.IsBound() && !CanApplyDamageToActor.Execute(Target))
    {
        return false;
    }

    OutResponse = FMADamageResponse();
    OutResponse.Target = Target;
    OutResponse.DamageCauser = DamageCauser;
    OutResponse.Instigator = Instigator;
    OutResponse.Damage = Damage;
    OutResponse.IncomingTags.AppendTags(AdditionalTags);
    OutResponse.Hit = Hit;

    const UMADamageTypeBase* DamageTypeInst = DamageType ? GetDefault<UMADamageTypeBase>(DamageType) : nullptr;
    if (DamageTypeInst)
    {
        DamageTypeInst->OnPreApplyDamage(OutResponse);
    }
    
    return IMADamageableInterface::Execute_OnIncomingDamage(Target, OutResponse) && OutResponse.IsApplyingDamage();
}