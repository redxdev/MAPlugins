// Copyright (c) MissiveArts LLC

using System.IO;
using UnrealBuildTool;

public class MADamage : ModuleRules
{
	public MADamage(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
			});
	}
}