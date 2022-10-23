// Copyright (c) MissiveArts LLC

#pragma once

#include "CoreMinimal.h"
#include "AITypes.h"

namespace MAGameplay::AI
{
	template<typename T>
	bool Compare(const T Left, const T Right, const EGenericAICheck Operator)
	{
		switch (Operator)
		{
		case EGenericAICheck::Equal:
			return Left == Right;

		case EGenericAICheck::NotEqual:
			return Left != Right;

		case EGenericAICheck::Less:
			return Left < Right;

		case EGenericAICheck::LessOrEqual:
			return Left <= Right;

		case EGenericAICheck::Greater:
			return Left > Right;

		case EGenericAICheck::GreaterOrEqual:
			return Left >= Right;

		default:
			ensureMsgf(false, TEXT("Unhandled operator %d"), Operator);
			return false;
		}

		return false;
	}
}