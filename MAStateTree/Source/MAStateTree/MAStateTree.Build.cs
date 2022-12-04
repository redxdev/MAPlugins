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
				"AIModule",
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayStateTreeModule",
				"GameplayTags",
				"GameplayTasks",
				"MAGameplay",
				"NavigationSystem",
				"StateTreeModule",
				"StructUtils",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("StateTreeEditorModule");
		}
	}
}