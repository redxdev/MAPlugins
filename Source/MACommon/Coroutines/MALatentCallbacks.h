// Copyright (c) MissiveArts LLC

#pragma once

#if WITH_MA_COROUTINES

#include "CoreMinimal.h"
#include "UE5Coro/LatentCallbacks.h"

#define MA_PRIVATE_LATENT_CALLBACK_INTERNAL(Name, Type, Line) const auto PREPROCESSOR_JOIN(Name, Line) = MACommon::Coroutines::Private::TLatentCallbackSyntaxSupport<UE5Coro::Latent::Type>() + [&]()
#define MA_ON_LATENT_ABORT MA_PRIVATE_LATENT_CALLBACK_INTERNAL(__latent_abort_,FOnActionAborted,__LINE__)
#define MA_ON_LATENT_DESTROY MA_PRIVATE_LATENT_CALLBACK_INTERNAL(__latent_destroy_,FOnObjectDestroyed,__LINE__)
#define MA_ON_LATENT_ABNORMAL_EXIT MA_PRIVATE_LATENT_CALLBACK_INTERNAL(__latent_abnormal_exit_,FOnActionAbortedOrDestroyed,__LINE__)
#define MA_ON_LATENT_EXIT ON_SCOPE_EXIT

namespace MACommon::Coroutines::Private
{
	template<typename TCallbackType>
	struct MACOMMON_API TLatentCallbackSyntaxSupport
	{
		template<typename FuncType>
		TCallbackType operator+(FuncType&& InFunc)
		{
			return TCallbackType(InFunc);
		}
	};
}

#endif