// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GFantasy : ModuleRules
{
	public GFantasy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
            "GameplayAbilities", 
			"GameplayTags", 
			"EnhancedInput",
			"GameplayTasks", 
			"UMG",
			//"OnlineSubsystem",
			//"OnlineSubsystemSteam",
			"Slate", 
			"SlateCore", 
			"AIModule", 
			"Landscape", 
			"Networking", 
			"NetCore", 
			"Sockets", 
			"AnimGraphRuntime", 
			"NavigationSystem", 
			"Niagara",
			"LevelSequence",
        });





	}
}
