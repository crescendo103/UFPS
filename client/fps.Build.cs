// Copyright Epic Games, Inc. All Rights Reserved.
/*
using UnrealBuildTool;

public class fps : ModuleRules
{
	public fps(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "Paper2D", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] {"Slate", "SlateCore"  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}*/
using UnrealBuildTool;

public class fps : ModuleRules
{
    public fps(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "Niagara",
                "Paper2D",
                "UMG",
                "Networking",
                "Sockets",
                "Voxel","GeometryCollectionEngine",
                "Chaos",
                "GeometryCollectionEngine",
                "FieldSystemEngine",
                "AIModule",
                "GameplayTasks",
                "NavigationSystem",              
                "Navmesh",
                "MediaAssets"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore",
                "Navmesh"
            }
        );

        PublicAdditionalLibraries.Add("ws2_32.lib");
    }
}
