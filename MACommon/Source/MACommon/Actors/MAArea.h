// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MAArea.generated.h"

// Defines an area for other systems to use.
// Currently only supports a box as the shape.
UCLASS(NotBlueprintable)
class MACOMMON_API AMAArea : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<class UShapeComponent> ShapeComponent;

public:
	AMAArea(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE UShapeComponent* GetShapeComponent() const { return ShapeComponent; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area")
	uint8 bProjectLocationsToNavmesh : 1;

	UFUNCTION(BlueprintPure=false, BlueprintCallable, Category = "Area")
	bool SelectRandomLocationWithinArea(FVector& OutLocation) const;

#if WITH_EDITOR
	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
#endif
};
