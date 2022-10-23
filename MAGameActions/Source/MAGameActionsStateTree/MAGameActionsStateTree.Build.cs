// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MAGameActionsStateTree : ModuleRules
{
	public MAGameActionsStateTree(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"AIModule",
				"GameplayTasks",
				"UE5Coro",
				"MAGameActions",
				"StateTreeModule",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"MACommon",
			});
	}
}