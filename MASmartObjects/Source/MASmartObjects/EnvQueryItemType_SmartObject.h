// Copyright (c) MissiveArts LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectSubsystem.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvQueryItemType_SmartObject.generated.h"

struct MASMARTOBJECTS_API FEnvQueryItemStorage_SmartObject
{
	FEnvQueryItemStorage_SmartObject()
	{
	}

	FEnvQueryItemStorage_SmartObject(USmartObjectSubsystem* InSmartObjectSubsystem, const FSmartObjectRequestResult& InObject)
		: SmartObjectSubsystem(InSmartObjectSubsystem), Object(InObject)
	{
	}

	FEnvQueryItemStorage_SmartObject(USmartObjectSubsystem* InSmartObjectSubsystem, FSmartObjectHandle ObjHandle, FSmartObjectSlotHandle SlotHandle)
		: SmartObjectSubsystem(InSmartObjectSubsystem), Object(ObjHandle, SlotHandle)
	{
	}

	TWeakObjectPtr<USmartObjectSubsystem> SmartObjectSubsystem;
	FSmartObjectRequestResult Object;
};

UCLASS()
class MASMARTOBJECTS_API UEnvQueryItemType_SmartObject : public UEnvQueryItemType_VectorBase
{
	GENERATED_BODY()

public:
	typedef const FEnvQueryItemStorage_SmartObject& FValueType;

	UEnvQueryItemType_SmartObject(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static FEnvQueryItemStorage_SmartObject GetValue(const uint8* RawData);
	static void SetValue(uint8* RawData, const FEnvQueryItemStorage_SmartObject& Value);

	static void SetContextHelper(FEnvQueryContextData& ContextData, const FEnvQueryItemStorage_SmartObject& SingleObject);
	static void SetContextHelper(FEnvQueryContextData& ContextData, const TArray<const FEnvQueryItemStorage_SmartObject> MultipleObjects);
	static void SetContextHelper(FEnvQueryContextData& ContextData, const TArray<FEnvQueryItemStorage_SmartObject> MultipleObjects);

	static void SetContextHelper(FEnvQueryContextData& ContextData, USmartObjectSubsystem* SmartObjectSubsystem, const FSmartObjectRequestResult& SingleObject);
	static void SetContextHelper(FEnvQueryContextData& ContextData, USmartObjectSubsystem* SmartObjectSubsystem, const TArray<const FSmartObjectRequestResult>& MultipleObjects);
	static void SetContextHelper(FEnvQueryContextData& ContextData, USmartObjectSubsystem* SmartObjectSubsystem, const TArray<FSmartObjectRequestResult>& MultipleObjects);

	virtual FVector GetItemLocation(const uint8* RawData) const override;
	virtual FRotator GetItemRotation(const uint8* RawData) const override;
};
