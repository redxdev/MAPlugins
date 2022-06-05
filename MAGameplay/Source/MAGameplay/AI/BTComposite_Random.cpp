// Copyright (c) MissiveArts LLC. All rights reserved.

#include "BTComposite_Random.h"

UBTComposite_Random::UBTComposite_Random()
{
	NodeName = "Random";
	bExcludeFailedChildren = true;
	bUseNodeDeactivationNotify = true;
}

int32 UBTComposite_Random::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	FBTRandomCompositeMemory* CompositeMemory = GetNodeMemory<FBTRandomCompositeMemory>(SearchData);
	if (bExcludeFailedChildren && LastResult == EBTNodeResult::Failed)
	{
		CompositeMemory->FailedChildren.Add(PrevChild);
	}

	int32 NextChildIdx = BTSpecialChild::ReturnToParent;
	if (GetChildrenNum() > 0 && (PrevChild == BTSpecialChild::NotInitialized || LastResult == EBTNodeResult::Failed))
	{

		// Grab weights from child nodes, calculate total weight
		float TotalWeight = 0.f;
		TArray<float> Weights;
		Weights.Reserve(GetChildrenNum());
		for (int32 ChildIndex = 0; ChildIndex < GetChildrenNum(); ++ChildIndex)
		{
			float ChildWeight = 1.f;
			if (bExcludeFailedChildren && CompositeMemory->FailedChildren.Contains(ChildIndex))
			{
				ChildWeight = 0.f;
			}
			else
			{
				const FBTCompositeChild& Child = Children[ChildIndex];
				for (UBTDecorator* Decorator : Child.Decorators)
				{
					if (const UBTDecorator_RandomWeight* RandomWeightDecorator = Cast<UBTDecorator_RandomWeight>(Decorator))
					{
						ChildWeight = RandomWeightDecorator->Weight;
						break;
					}
				}
			}

			Weights.Add(ChildWeight);
			if (ChildWeight > 0.f)
			{
				TotalWeight += ChildWeight;
			}
		}

		if (TotalWeight > 0.f)
		{
			float RandomValue = FMath::FRandRange(0.f, TotalWeight);
			int32 Index = 0;
			while (Index < GetChildrenNum())
			{
				float ChildWeight = Weights[Index];
				if (ChildWeight > 0)
				{
					RandomValue -= ChildWeight;
					if (RandomValue <= 0.f)
					{
						break;
					}
				}

				++Index;
			}

			NextChildIdx = Index;
		}
	}

	return NextChildIdx;
}

void UBTComposite_Random::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	Super::InitializeMemory(OwnerComp, NodeMemory, InitType);

	FBTRandomCompositeMemory* CompositeMemory = CastInstanceNodeMemory<FBTRandomCompositeMemory>(NodeMemory);
	if (InitType == EBTMemoryInit::Initialize)
	{
		CompositeMemory->FailedChildren.Empty();
	}
}

uint16 UBTComposite_Random::GetInstanceMemorySize() const
{
	return sizeof(FBTRandomCompositeMemory);
}

void UBTComposite_Random::NotifyNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type& NodeResult) const
{
	Super::NotifyNodeDeactivation(SearchData, NodeResult);

	FBTRandomCompositeMemory* CompositeMemory = GetNodeMemory<FBTRandomCompositeMemory>(SearchData);
	CompositeMemory->FailedChildren.Empty();
}

UBTDecorator_RandomWeight::UBTDecorator_RandomWeight()
{
	NodeName = "Random Weight";
	bAllowAbortChildNodes = false;
	bAllowAbortLowerPri = false;
}

FString UBTDecorator_RandomWeight::GetStaticDescription() const
{
	return FString::Printf(TEXT("Weight: %f"), Weight);
}
