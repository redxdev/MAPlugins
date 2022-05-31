// Copyright (c) MissiveArts LLC

#include "MAGameplay.h"
#include "Modules/ModuleManager.h"

class FMAGameplayModule : public IModuleInterface
{
};

IMPLEMENT_MODULE(FMAGameplayModule, MAGameplay);

DEFINE_LOG_CATEGORY(LogMAGameplay);