// Copyright (c) MissiveArts LLC


#include "MAArea.h"
#include "NavigationSystem.h"
#include "Components/BoxComponent.h"


AMAArea::AMAArea(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ShapeComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("ShapeComponent"));
	if (ShapeComponent)
	{
		ShapeComponent->SetCollisionProfileName("NoCollision");
		RootComponent = ShapeComponent;
	}

	bProjectLocationsToNavmesh = true;
}

bool AMAArea::SelectRandomLocationWithinArea(FVector& OutLocation) const
{
	// Only box component is supported... for now.
	if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(ShapeComponent))
	{
		FBox BoundsBox = BoxComponent->Bounds.GetBox();
		FVector LocalLocation = FMath::RandPointInBox(BoundsBox);
		const FTransform& Transform = ShapeComponent->GetComponentTransform();
		OutLocation = Transform.TransformVector(LocalLocation);

		if (bProjectLocationsToNavmesh)
		{
			if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
			{
				FNavLocation NavLocation;
				if (NavSys->ProjectPointToNavigation(OutLocation, NavLocation))
				{
					OutLocation = NavLocation.Location;
					FVector NavLocationInLocalSpace = Transform.InverseTransformVector(NavLocation.Location);
					if (BoundsBox.IsInside(NavLocationInLocalSpace))
					{
						OutLocation = NavLocation.Location;
						return true;
					}
				}
			}

			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

#if WITH_EDITOR

void AMAArea::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(ShapeComponent))
	{
		const FVector ModifiedScale = DeltaScale * ( AActor::bUsePercentageBasedScaling ? 500.0f : 5.0f );

		if ( bCtrlDown )
		{
			// CTRL+Scaling modifies trigger collision height.  This is for convenience, so that height
			// can be changed without having to use the non-uniform scaling widget (which is
			// inaccessable with spacebar widget cycling).
			FVector Extent = BoxComponent->GetUnscaledBoxExtent() + FVector(0, 0, ModifiedScale.X);
			Extent.Z = FMath::Max(0.0f, Extent.Z);
			BoxComponent->SetBoxExtent(Extent);
		}
		else
		{
			FVector Extent = BoxComponent->GetUnscaledBoxExtent() + FVector(ModifiedScale.X, ModifiedScale.Y, ModifiedScale.Z);
			Extent.X = FMath::Max(0.0f, Extent.X);
			Extent.Y = FMath::Max(0.0f, Extent.Y);
			Extent.Z = FMath::Max(0.0f, Extent.Z);
			BoxComponent->SetBoxExtent(Extent);
		}
	}
	else
	{
		Super::EditorApplyScale(DeltaScale, PivotLocation, bAltDown, bShiftDown, bCtrlDown);
	}
}

#endif