// Copyright (c) MissiveArts LLC

#include "MAStateTree.h"
#include "Modules/ModuleManager.h"

class FMAStateTreeModule : public IModuleInterface
{
};

IMPLEMENT_MODULE(FMAStateTreeModule, MAStateTree);

DEFINE_LOG_CATEGORY(LogMAStateTree);