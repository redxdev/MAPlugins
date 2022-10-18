// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MAStateTree : ModuleRules
{
	public MAStateTree(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule",
				"NavigationSystem",
				"StateTreeModule",
				"GameplayStateTreeModule",
				"GameplayTasks",
				"GameplayTags",
			});
	}
}