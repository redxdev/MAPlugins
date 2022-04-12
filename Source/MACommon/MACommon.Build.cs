// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MACommon : ModuleRules
{
	public const bool bEnableCoroutines = true;
	
	public MACommon(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
			});

		// TODO: better way of telling whether UE5Coro is installed...
		string pluginDir = Path.Combine(PluginDirectory, "..");
		if (Directory.Exists(Path.Combine(pluginDir, "UE5Coro")))
		{
			PublicDependencyModuleNames.Add("UE5Coro");
			PublicDefinitions.Add("WITH_MA_COROUTINES=1");
		}
	}
}