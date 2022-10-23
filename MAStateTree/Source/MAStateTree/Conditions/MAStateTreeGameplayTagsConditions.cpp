// Copyright (c) MissiveArts LLC

#include "MAStateTreeGameplayTagsConditions.h"

#include "GameplayTagAssetInterface.h"

FMAStateTreeGameplayTagInterfaceMatchCondition::FMAStateTreeGameplayTagInterfaceMatchCondition()
{
	bInvert = false;
}

bool FMAStateTreeGameplayTagInterfaceMatchCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (IsValid(InstanceData.Target))
	{
		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(InstanceData.Target))
		{
			const bool bResult = TagInterface->HasMatchingGameplayTag(InstanceData.Tag);
			if (bInvert)
			{
				return !bResult;
			}

			return bResult;
		}
	}

	return false;
}

FMAStateTreeGameplayTagInterfaceContainerMatchCondition::FMAStateTreeGameplayTagInterfaceContainerMatchCondition()
{
	bInvert = false;
}

bool FMAStateTreeGameplayTagInterfaceContainerMatchCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (IsValid(InstanceData.Target))
	{
		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(InstanceData.Target))
		{
			bool bResult;
			switch (MatchType)
			{
			default:
				checkNoEntry();
				return false;

			case EGameplayContainerMatchType::Any:
				bResult = TagInterface->HasAnyMatchingGameplayTags(InstanceData.TagContainer);
				break;

			case EGameplayContainerMatchType::All:
				bResult = TagInterface->HasAllMatchingGameplayTags(InstanceData.TagContainer);
				break;
			}

			if (bInvert)
			{
				return !bResult;
			}

			return bResult;
		}
	}

	return false;
}

FMAStateTreeGameplayTagInterfaceQueryMatchCondition::FMAStateTreeGameplayTagInterfaceQueryMatchCondition()
{
	bInvert = false;
}

bool FMAStateTreeGameplayTagInterfaceQueryMatchCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (IsValid(InstanceData.Target))
	{
		if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(InstanceData.Target))
		{
			FGameplayTagContainer Container;
			TagInterface->GetOwnedGameplayTags(Container);

			bool bResult = TagQuery.Matches(Container);
			if (bInvert)
			{
				return !bResult;
			}

			return bResult;
		}
	}

	return false;
}
