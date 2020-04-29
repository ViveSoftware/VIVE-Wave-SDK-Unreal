// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

using System.IO;
using System.Diagnostics;

namespace UnrealBuildTool.Rules
{
	public class WaveVREditor : ModuleRules
	{
		public WaveVREditor(ReadOnlyTargetRules Target) : base(Target)
		{
			Debug.Print("Build the WaveVR Editor Module");

			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);

			PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Projects",
					"InputCore",
					"UnrealEd",
					"LevelEditor",
					"Core",
					"CoreUObject",
					"Engine",
					"WVR",
					"Slate",
					"SlateCore",
					"Sockets"
				}
			);
			if(Target.bBuildEditor == true)
			{
				PrivateDependencyModuleNames.Add("UnrealEd");
			}
		}
	}
}
