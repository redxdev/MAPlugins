// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MAGameActions : ModuleRules
{
	public MAGameActions(ReadOnlyTargetRules Target) : base(Target)
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
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"MACommon",
			});
	}
}