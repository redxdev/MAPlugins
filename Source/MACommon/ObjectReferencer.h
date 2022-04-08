// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"

#define MA_REF_PASTE(x, y) x ## y

// Use existing local UObject pointers as strong pointers for the duration of the current scope.
#define MA_STRONG_REF(...) const MACommon::Private::FStrongReferencer PREPROCESSOR_JOIN(__scoped_strong_ref_, __LINE__)(__VA_ARGS__)

// Use existing local UObject pointers as weak pointers for the duration of the current scope. They will be set to null if the object is deleted.
#define MA_WEAK_REF(...) const MACommon::Private::FWeakReferencer PREPROCESSOR_JOIN(__scoped_weak_ref_, __LINE__)(__VA_ARGS__)

namespace MACommon::Private
{
	class FBaseReferencer
	{
	public:
		template<typename... TArgs>
		FBaseReferencer(TArgs&... Args)
		{
			Objects.Reserve(sizeof...(TArgs));
			InitializeReferences(Args...);
		}

		virtual ~FBaseReferencer() {}

		UE_NONCOPYABLE(FBaseReferencer);

	protected:
		TArray<UObject**> Objects;

	private:
		template<typename TFirst, typename... TArgs, typename = TEnableIf<std::is_base_of_v<UObject, TFirst>>>
		void InitializeReferences(TFirst*& InObject, TArgs&... Args)
		{
			InitObjectReference(reinterpret_cast<UObject*&>(InObject));
			InitializeReferences(Args...);
		}
	
		void InitObjectReference(UObject*& InObject)
		{
			if (InObject != nullptr)
			{
				Objects.Add(&InObject);
			}
		}
		
		template<typename TFirst, typename... TArgs, typename = TEnableIf<std::is_base_of_v<UObject, TFirst>>>
		void InitializeReferences(TObjectPtr<TFirst>& InObject, TArgs&... Args)
		{
			// Possible bug: object isn't resolved but will be later
			if (IsObjectHandleResolved(InObject.GetHandle()))
			{
				InitObjectReference(reinterpret_cast<UObject*&>(InObject));
			}

			InitializeReferences(Args...);
		}

		void InitializeReferences() {}
	};
	
	class MACOMMON_API FStrongReferencer : public FBaseReferencer, public FGCObject
	{
	public:
		template<typename... TArgs>
		FStrongReferencer(TArgs&... Args) : FBaseReferencer(Args...), FGCObject() {}

		virtual ~FStrongReferencer() {}

		virtual FString GetReferencerName() const override { return TEXT("FStrongReferencer"); }
		
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			// Can't pass the whole array in as the original refs won't be nulled out.
			for (UObject** Object : Objects)
			{
				if (Object != nullptr && (*Object) != nullptr)
				{
					Collector.AddReferencedObject(*Object);
				}
			}
		}
	};

	class MACOMMON_API FWeakReferencer : public FBaseReferencer, public FGCObject
	{
	public:
		template<typename... TArgs>
		FWeakReferencer(TArgs&... Args) : FBaseReferencer(Args...), FGCObject() {}
		
		virtual ~FWeakReferencer() {}

		virtual FString GetReferencerName() const override { return TEXT("FWeakReferencer"); }
		
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			// Can't pass the whole array in as the original refs won't be nulled out.
			for (UObject** Object : Objects)
			{
				if (Object != nullptr && (*Object) != nullptr)
				{
					Collector.MarkWeakObjectReferenceForClearing(Object);
				}
			}
		}
	};
}