// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Forgotten : ModuleRules
{
	public Forgotten(ReadOnlyTargetRules target) : base(target)
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
			"GameplayTags",
			"UMG",
			"Slate",
			"SlateCore",
			"UnrealVoxta"
		]);

		if (target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(["UnrealEd"]);
        }
	}
}
