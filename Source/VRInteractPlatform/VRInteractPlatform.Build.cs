// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VRInteractPlatform : ModuleRules
{
	public VRInteractPlatform(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(
            new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "HeadMountedDisplay", 
            "Sockets", 
            "Networking", 
            "ImageWrapper", 
            "CinematicCamera",
            "RenderCore"
        }
            );

        PrivateIncludePaths.AddRange(
            new string[] {
                "VRInteractPlatform/cnpy",
                "ProceduralMeshComponent"
            }
        );
        PublicIncludePathModuleNames.AddRange(
            new string[] { 
             }
            );

        PublicIncludePaths.AddRange(
            new string[] { 
            
            
             }
            );

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }

    }
}
