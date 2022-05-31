// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MAGameplay : ModuleRules
{
	public MAGameplay(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"AIModule",
			});
	}
}