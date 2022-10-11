// Copyright (c) MissiveArts LLC

#include "BTTask_FindAndUseSmartObjectEx.h"
#include "AIController.h"
#include "BlackboardKeyType_SOClaimHandle.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayBehaviorSmartObjectBehaviorDefinition.h"
#include "AI/AITask_UseGameplayBehaviorSmartObject.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "VisualLogger/VisualLogger.h"

UBTTask_FindAndUseSmartObjectEx::UBTTask_FindAndUseSmartObjectEx()
{
	NodeName = "Find and Use Smart Object (Ex)";
	Radius = 500.f;
	SearchCenter.SelectedKeyName = FBlackboard::KeySelf;
	SortMode = ESmartObjectSortMode::First;
}

EBTNodeResult::Type UBTTask_FindAndUseSmartObjectEx::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	UWorld* World = GetWorld();
	USmartObjectSubsystem* Subsystem = USmartObjectSubsystem::GetCurrent(World);
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (Subsystem == nullptr || MyController == nullptr
		|| MyController->GetPawn() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector SearchLocation;
	if (!OwnerComp.GetBlackboardComponent()->GetLocationFromEntry(SearchCenter.GetSelectedKeyID(), SearchLocation))
	{
		return EBTNodeResult::Failed;
	}

	FBTUseSOExTaskMemory* MyMemory = reinterpret_cast<FBTUseSOExTaskMemory*>(NodeMemory);
	MyMemory->TaskInstance.Reset();

	AActor& Avatar = *MyController->GetPawn();

	// Create filter
	FSmartObjectRequestFilter Filter;
	Filter.ActivityRequirements = ActivityRequirements;
	Filter.BehaviorDefinitionClasses.Add(UGameplayBehaviorSmartObjectBehaviorDefinition::StaticClass());

	const IGameplayTagAssetInterface* TagsSource = Cast<const IGameplayTagAssetInterface>(&Avatar);
	if (TagsSource != nullptr)
	{
		TagsSource->GetOwnedGameplayTags(Filter.UserTags);
	}

	// Create request
	FSmartObjectRequest Request(FBox(SearchLocation, SearchLocation).ExpandBy(FVector(Radius), FVector(Radius)), Filter);
	TArray<FSmartObjectRequestResult> Results;

	if (Subsystem->FindSmartObjects(Request, Results))
	{
		FSmartObjectClaimHandle ClaimHandle = SortAndClaimSlot(Subsystem, Results, SearchLocation);
		if (ClaimHandle.IsValid())
		{
			UAITask_UseGameplayBehaviorSmartObject* UseSOTask = NewBTAITask<UAITask_UseGameplayBehaviorSmartObject>(OwnerComp);
			UseSOTask->SetClaimHandle(ClaimHandle);
			UseSOTask->ReadyForActivation();

			NodeResult = EBTNodeResult::InProgress;
			UE_VLOG_UELOG(MyController, LogSmartObject, Verbose, TEXT("%s claimed smart object: %s"), *GetNodeName(), *LexToString(ClaimHandle));
		}

		UE_CVLOG_UELOG(NodeResult == EBTNodeResult::Failed, MyController, LogSmartObject, Warning, TEXT("%s failed to claim smart object"), *GetNodeName());
	}
	else
	{
		UE_VLOG_UELOG(MyController, LogSmartObject
			, Verbose, TEXT("%s failed to find smart objects for request: %s")
			, *GetNodeName(), *Avatar.GetName());
	}

	return NodeResult;
}

EBTNodeResult::Type UBTTask_FindAndUseSmartObjectEx::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

void UBTTask_FindAndUseSmartObjectEx::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
}

FSmartObjectClaimHandle UBTTask_FindAndUseSmartObjectEx::SortAndClaimSlot(USmartObjectSubsystem* Subsystem, TArray<FSmartObjectRequestResult>& InResults, const FVector& SearchLocation)
{
	FSmartObjectClaimHandle Claim;
	switch (SortMode)
	{
	default:
		checkNoEntry();
		break;

	case ESmartObjectSortMode::First:
		for (const FSmartObjectRequestResult& Result : InResults)
		{
			if (Result.IsValid())
			{
				Claim = Subsystem->Claim(Result);
				if (Claim.IsValid())
				{
					break;
				}
			}
		}

		break;

	case ESmartObjectSortMode::Random:
		while (!Claim.IsValid() && InResults.Num() > 0)
		{
			int32 Index = FMath::RandHelper(InResults.Num());
			const FSmartObjectRequestResult& Result = InResults[Index];
			if (Result.IsValid())
			{
				Claim = Subsystem->Claim(Result);
				if (Claim.IsValid())
				{
					break;
				}
			}

			InResults.RemoveAtSwap(Index, 1, false);
		}

		break;

	case ESmartObjectSortMode::Closest:
	case ESmartObjectSortMode::Furthest:
	case ESmartObjectSortMode::Closest25Percent:
	case ESmartObjectSortMode::Furthest25Percent:
	case ESmartObjectSortMode::ClosestWeighted:
	case ESmartObjectSortMode::FurthestWeighted:
		{
			InResults.Sort([&](const auto& A, const auto& B)
			{
				TOptional<FVector> Loc = Subsystem->GetSlotLocation(A);
				if (!Loc.IsSet())
				{
					return false;
				}

				float ADist = FVector::DistSquared(Loc.GetValue(), SearchLocation);

				Loc = Subsystem->GetSlotLocation(B);
				if (!Loc.IsSet())
				{
					return true;
				}

				float BDist = FVector::DistSquared(Loc.GetValue(), SearchLocation);
				return SortMode == ESmartObjectSortMode::Closest ? (ADist < BDist) : (BDist < ADist);
			});

			if (SortMode == ESmartObjectSortMode::Closest || SortMode == ESmartObjectSortMode::Furthest)
			{
				for (const FSmartObjectRequestResult& Result : InResults)
				{
					if (Result.IsValid())
					{
						Claim = Subsystem->Claim(Result);
						if (Claim.IsValid())
						{
							break;
						}
					}
				}
			}
			else if (SortMode == ESmartObjectSortMode::Closest25Percent || SortMode == ESmartObjectSortMode::Furthest25Percent)
			{
				while (!Claim.IsValid() && InResults.Num() > 0)
				{
					int32 EffectiveNum = InResults.Num() / 4;
					int32 Index = EffectiveNum > 1 ? FMath::RandHelper(EffectiveNum) : 0;
					const FSmartObjectRequestResult& Result = InResults[Index];
					if (Result.IsValid())
					{
						Claim = Subsystem->Claim(Result);
						if (Claim.IsValid())
						{
							break;
						}
					}

					InResults.RemoveAt(Index, 1, false);
				}
			}
			else
			{
				while (!Claim.IsValid() && InResults.Num() > 0)
				{
					int32 MaxValue = (InResults.Num() * (InResults.Num() + 1)) / 2;
					int32 Value = MaxValue > 1 ? FMath::RandHelper(MaxValue + 1) : 0;

					// There's probably a faster way to do this than iterating
					// Ideally this would be based on distance rather than index anyway, but it probably doesn't matter _that_ much.
					int32 Index = 0;
					while (Index < InResults.Num())
					{
						Value -= InResults.Num() - Index;
						if (Value <= 0)
						{
							break;
						}

						++Index;
					}

					const FSmartObjectRequestResult& Result = InResults[Index];
					if (Result.IsValid())
					{
						Claim = Subsystem->Claim(Result);
						if (Claim.IsValid())
						{
							break;
						}
					}

					InResults.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
	}

	return Claim;
}

FString UBTTask_FindAndUseSmartObjectEx::GetStaticDescription() const
{
	FString Result = FString::Printf(
		TEXT("Search Location: %s\nSort: %s"),
		*SearchCenter.SelectedKeyName.ToString(),
		*StaticEnum<ESmartObjectSortMode>()->GetDisplayNameTextByValue(static_cast<uint8>(SortMode)).ToString());
	if (ActivityRequirements.IsEmpty() == false)
	{
		Result += FString::Printf(TEXT("\nObject requirements: %s")
			, *ActivityRequirements.GetDescription());
	}

	return Result;
}

void UBTTask_FindAndUseSmartObjectEx::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* Blackboard = GetBlackboardAsset();
	if (Blackboard)
	{
		SearchCenter.ResolveSelectedKey(*Blackboard);
	}
}
