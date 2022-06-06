// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MASmartObjects : ModuleRules
{
	public MASmartObjects(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule",
				"SmartObjectsModule",
				"GameplayBehaviorsModule",
				"GameplayTasks",
				"GameplayTags",
			});
	}
}