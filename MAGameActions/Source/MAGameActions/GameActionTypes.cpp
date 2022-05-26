// Copyright (c) MissiveArts LLC


#include "GameActionTypes.h"

const FGameActionHandle FGameActionHandle::Invalid = FGameActionHandle();

FGameActionHandle::FGameActionHandle()
{
    this->Id = 0;
}

FGameActionHandle::FGameActionHandle(uint32 Id)
{
    this->Id = Id;
}

bool UGameActionHandleLibrary::IsValid(const FGameActionHandle& Handle)
{
    return Handle.IsValid();
}

FGameActionHandle UGameActionHandleLibrary::GetInvalidHandle()
{
    return FGameActionHandle::Invalid;
}
