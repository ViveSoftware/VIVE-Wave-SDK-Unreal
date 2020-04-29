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

namespace UnrealBuildTool.Rules
{
	public class WaveVRGesture : ModuleRules
	{
		public WaveVRGesture(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
			var openglDrvPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\OpenGLDrv\Private");
			var openglPath = Path.Combine(EngineDir, @"Source\ThirdParty\OpenGL");
			var rendererPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\Renderer\Private");

			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
				);

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
					"WaveVR",
					"WVR",
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"InputDevice",
					"ApplicationCore",
					"HeadMountedDisplay",
					"OpenGLDrv",
					"UMG"
				});

			PrivateIncludePaths.AddRange(
				new string[] {
					"WaveVR/Private",
					openglDrvPrivatePath,
					openglPath,
					rendererPrivatePath
				});

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}
