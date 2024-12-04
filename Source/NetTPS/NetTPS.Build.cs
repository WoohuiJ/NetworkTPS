// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NetTPS : ModuleRules
{
	public NetTPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SlateCore","UMG", "EnhancedInput","OnlineSubsystem", "OnlineSubsystemSteam" });
	}
}
