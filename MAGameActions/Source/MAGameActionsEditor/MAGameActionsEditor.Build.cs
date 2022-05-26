// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MAGameActionsEditor : ModuleRules
{
	public MAGameActionsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"BlueprintGraph",
				"UnrealEd",
				"MAGameActions",
			});
	}
}