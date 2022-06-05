// Copyright (c) MissiveArts LLC. All rights reserved.

#include "EnvQueryItemType_SmartObject.h"
#include "AITypes.h"
#include "SmartObjectComponent.h"

UEnvQueryItemType_SmartObject::UEnvQueryItemType_SmartObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ValueSize = sizeof(FEnvQueryItemStorage_SmartObject);
}

FEnvQueryItemStorage_SmartObject UEnvQueryItemType_SmartObject::GetValue(const uint8* RawData)
{
	return GetValueFromMemory<FEnvQueryItemStorage_SmartObject>(RawData);
}

void UEnvQueryItemType_SmartObject::SetValue(uint8* RawData, const FEnvQueryItemStorage_SmartObject& Value)
{
	SetValueInMemory<FEnvQueryItemStorage_SmartObject>(RawData, Value);
}

void UEnvQueryItemType_SmartObject::SetContextHelper(FEnvQueryContextData& ContextData, const FEnvQueryItemStorage_SmartObject& SingleObject)
{
	ContextData.ValueType = UEnvQueryItemType_SmartObject::StaticClass();
	ContextData.NumValues = 1;
	ContextData.RawData.SetNumUninitialized(sizeof(FEnvQueryItemStorage_SmartObject));
	SetValue(ContextData.RawData.GetData(), SingleObject);
}

void UEnvQueryItemType_SmartObject::SetContextHelper(FEnvQueryContextData& ContextData, const TArray<const FEnvQueryItemStorage_SmartObject> MultipleObjects)
{
	ContextData.ValueType = UEnvQueryItemType_SmartObject::StaticClass();
	ContextData.NumValues = MultipleObjects.Num();
	ContextData.RawData.SetNumUninitialized(sizeof(FEnvQueryItemStorage_SmartObject) * MultipleObjects.Num());

	uint8* RawData = (uint8*)ContextData.RawData.GetData();
	for (const FEnvQueryItemStorage_SmartObject& Obj : MultipleObjects)
	{
		SetValue(RawData, Obj);
		RawData += sizeof(FEnvQueryItemStorage_SmartObject);
	}
}

void UEnvQueryItemType_SmartObject::SetContextHelper(FEnvQueryContextData& ContextData, const TArray<FEnvQueryItemStorage_SmartObject> MultipleObjects)
{
	ContextData.ValueType = UEnvQueryItemType_SmartObject::StaticClass();
	ContextData.NumValues = MultipleObjects.Num();
	ContextData.RawData.SetNumUninitialized(sizeof(FEnvQueryItemStorage_SmartObject) * MultipleObjects.Num());

	uint8* RawData = (uint8*)ContextData.RawData.GetData();
	for (const FEnvQueryItemStorage_SmartObject& Obj : MultipleObjects)
	{
		SetValue(RawData, Obj);
		RawData += sizeof(FEnvQueryItemStorage_SmartObject);
	}
}

void UEnvQueryItemType_SmartObject::SetContextHelper(FEnvQueryContextData& ContextData, USmartObjectSubsystem* SmartObjectSubsystem, const FSmartObjectRequestResult& SingleObject)
{
	SetContextHelper(ContextData, FEnvQueryItemStorage_SmartObject(SmartObjectSubsystem, SingleObject));
}

void UEnvQueryItemType_SmartObject::SetContextHelper(FEnvQueryContextData& ContextData, USmartObjectSubsystem* SmartObjectSubsystem, const TArray<const FSmartObjectRequestResult>& MultipleObjects)
{
	ContextData.ValueType = UEnvQueryItemType_SmartObject::StaticClass();
	ContextData.NumValues = MultipleObjects.Num();
	ContextData.RawData.SetNumUninitialized(sizeof(FEnvQueryItemStorage_SmartObject) * MultipleObjects.Num());

	uint8* RawData = (uint8*)ContextData.RawData.GetData();
	for (const FSmartObjectRequestResult& Obj : MultipleObjects)
	{
		SetValue(RawData, FEnvQueryItemStorage_SmartObject(SmartObjectSubsystem, Obj));
		RawData += sizeof(FEnvQueryItemStorage_SmartObject);
	}
}

void UEnvQueryItemType_SmartObject::SetContextHelper(FEnvQueryContextData& ContextData, USmartObjectSubsystem* SmartObjectSubsystem, const TArray<FSmartObjectRequestResult>& MultipleObjects)
{
	ContextData.ValueType = UEnvQueryItemType_SmartObject::StaticClass();
	ContextData.NumValues = MultipleObjects.Num();
	ContextData.RawData.SetNumUninitialized(sizeof(FEnvQueryItemStorage_SmartObject) * MultipleObjects.Num());

	uint8* RawData = (uint8*)ContextData.RawData.GetData();
	for (const FSmartObjectRequestResult& Obj : MultipleObjects)
	{
		SetValue(RawData, FEnvQueryItemStorage_SmartObject(SmartObjectSubsystem, Obj));
		RawData += sizeof(FEnvQueryItemStorage_SmartObject);
	}
}

FVector UEnvQueryItemType_SmartObject::GetItemLocation(const uint8* RawData) const
{
	const FEnvQueryItemStorage_SmartObject Storage = GetValue(RawData);
	if (USmartObjectSubsystem* SO = Storage.SmartObjectSubsystem.Get())
	{
		if (Storage.Object.IsValid())
		{
			TOptional<FVector> Location = SO->GetSlotLocation(Storage.Object);
			if (Location.IsSet())
			{
				return Location.GetValue();
			}
		}
	}

	return FAISystem::InvalidLocation;
}

FRotator UEnvQueryItemType_SmartObject::GetItemRotation(const uint8* RawData) const
{
	const FEnvQueryItemStorage_SmartObject Storage = GetValue(RawData);
	if (USmartObjectSubsystem* SO = Storage.SmartObjectSubsystem.Get())
	{
		if (Storage.Object.IsValid())
		{
			TOptional<FTransform> Transform = SO->GetSlotTransform(Storage.Object);
			if (Transform.IsSet())
			{
				return Transform->Rotator();
			}
		}
	}

	return FAISystem::InvalidRotation;
}