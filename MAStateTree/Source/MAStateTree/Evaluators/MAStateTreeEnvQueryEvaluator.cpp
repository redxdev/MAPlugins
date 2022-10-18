// Copyright (c) MissiveArts LLC


#include "MAStateTreeEnvQueryEvaluator.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "MAGameplay/MAStateTree.h"
#include "VisualLogger/VisualLogger.h"

FMAStateTreeEnvQueryEvaluator::FMAStateTreeEnvQueryEvaluator()
{
	bClearResultsOnQueryFailure = true;
}

void FMAStateTreeEnvQueryEvaluator::TreeStop(FStateTreeExecutionContext& Context) const
{
	InstanceDataType& InstanceData = Context.GetInstanceData<InstanceDataType>(*this);
	Reset(Context, InstanceData);
}

void FMAStateTreeEnvQueryEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	UWorld* World = Context.GetWorld();
	InstanceDataType& InstanceData = Context.GetInstanceData<InstanceDataType>(*this);

	if (InstanceData.ResultData->bFinished)
	{
		if (ResultUpdatedEventTag.IsValid())
		{
			if (InstanceData.ResultActor != InstanceData.ResultData->ResultActor || (!InstanceData.ResultActor.IsValid() && InstanceData.ResultLocation != InstanceData.ResultData->ResultLocation))
			{
				Context.SendEvent(ResultUpdatedEventTag);
			}
		}

		InstanceData.RequestId = INDEX_NONE;
		InstanceData.ResultActor = InstanceData.ResultData->ResultActor;
		InstanceData.ResultLocation = InstanceData.ResultData->ResultLocation;
		InstanceData.bResult = InstanceData.ResultData->bResult;
		InstanceData.ResultData->bFinished = false;

		InstanceData.NextUpdate = World->GetTimeSeconds() + (InstanceData.bResult ? TickInterval : RetryCooldown);
	}

	if (InstanceData.RequestId >= 0 || InstanceData.NextUpdate > World->GetTimeSeconds())
	{
		// UE_VLOG(Context.GetOwner(), LogMAStateTree, Verbose, TEXT("EnvQuery eval skipped: request in progress or haven't hit next update time"));
		return;
	}

	InstanceData.NextUpdate = 0.0;
	RunQuery(Context, InstanceData);
}

void FMAStateTreeEnvQueryEvaluator::Reset(FStateTreeExecutionContext& Context, InstanceDataType& InstanceData) const
{
	if (InstanceData.RequestId >= 0)
	{
		if (UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(Context.GetWorld()))
		{
			QueryManager->AbortQuery(InstanceData.RequestId);
		}
	}

	InstanceData.ResultLocation = FAISystem::InvalidLocation;
	InstanceData.ResultActor = nullptr;
	InstanceData.RequestId = INDEX_NONE;
	InstanceData.bResult = false;
	InstanceData.ResultData->bFinished = false;
	InstanceData.ResultData->bResult = false;
	// Don't reset NextUpdate
}

void FMAStateTreeEnvQueryEvaluator::RunQuery(FStateTreeExecutionContext& Context, InstanceDataType& InstanceData) const
{
	InstanceData.RequestId = INDEX_NONE;
	InstanceData.ResultData->bFinished = false;

	UWorld* World = Context.GetWorld();

	if (!IsValid(InstanceData.QueryTemplate) || !IsValid(InstanceData.Owner))
	{
		UE_VLOG(Context.GetOwner(), LogMAStateTree, Warning, TEXT("EnvQuery run skipped: invalid query template or owner"));
		InstanceData.NextUpdate = World->GetTimeSeconds() + RetryCooldown;

		if (bClearResultsOnQueryFailure)
		{
			InstanceData.ResultActor = nullptr;
			InstanceData.ResultLocation = FAISystem::InvalidLocation;
		}

		InstanceData.bResult = false;
		return;
	}

	auto OnQueryCompleted = [ResultData=InstanceData.ResultData](TSharedPtr<FEnvQueryResult> QueryResult)
	{
		UE_VLOG(QueryResult->Owner.Get(), LogMAStateTree, Verbose, TEXT("EnvQuery request completed with %i items"), QueryResult->Items.Num());

		ResultData->bFinished = true;
		ResultData->bResult = false;
		if (QueryResult.IsValid() && !QueryResult->Items.IsEmpty() && IsValid(QueryResult->ItemType))
		{
			uint8* Data = QueryResult->RawData.GetData() + QueryResult->Items[0].DataOffset;

			UEnvQueryItemType* ItemTypeCDO = QueryResult->ItemType->GetDefaultObject<UEnvQueryItemType>();
			bool bSetActor = false;
			if (UEnvQueryItemType_ActorBase* ActorItemType = Cast<UEnvQueryItemType_ActorBase>(ItemTypeCDO))
			{
				ResultData->ResultActor = ActorItemType->GetActor(Data);
				ResultData->bResult = true;
			}
			else
			{
				ResultData->ResultActor = nullptr;
			}

			if (UEnvQueryItemType_VectorBase* VectorItemType = Cast<UEnvQueryItemType_VectorBase>(ItemTypeCDO))
			{
				ResultData->ResultLocation = VectorItemType->GetItemLocation(Data);
				ResultData->bResult = true;
			}

			if (!ResultData->bResult)
			{
				UE_VLOG(QueryResult->Owner.Get(), LogMAStateTree, Warning, TEXT("Failed to retrieve EQS query result (unknown item type %s)"), *GetNameSafe(QueryResult->ItemType));
			}
		}
	};

	FEnvQueryRequest QueryRequest(InstanceData.QueryTemplate, InstanceData.Owner);
	QueryRequest.SetNamedParams(InstanceData.QueryConfig);
	InstanceData.RequestId = QueryRequest.Execute(
		RunMode,
		FQueryFinishedSignature::CreateWeakLambda(World, OnQueryCompleted));

	UE_VLOG(Context.GetOwner(), LogMAStateTree, Verbose, TEXT("EnvQuery started request id %i"), InstanceData.RequestId);

	if (InstanceData.RequestId < 0)
	{
		InstanceData.NextUpdate = World->GetTimeSeconds() + RetryCooldown;
	}
}
