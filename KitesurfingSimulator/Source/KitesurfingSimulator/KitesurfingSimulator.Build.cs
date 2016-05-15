// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KitesurfingSimulator : ModuleRules
{
	public KitesurfingSimulator(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Landscape", "OceanPlugin", "Wiimote" });
	}
}
