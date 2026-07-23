// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Forgotten : ModuleRules
{
	public Forgotten(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange([
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"GameplayTags"
		]);

		PrivateDependencyModuleNames.AddRange([]);
	}
}
