// Copyright (c) MissiveArts LLC

using UnrealBuildTool;

public class MACommon : ModuleRules
{
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
	}
}