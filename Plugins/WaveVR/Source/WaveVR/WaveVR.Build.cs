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
	public class WaveVR : ModuleRules
	{
		public WaveVR(ReadOnlyTargetRules Target) : base(Target)
		{
			Debug.Print("Build the WaveVR Plugin");
			//bUseRTTI = true; // for dynamic_cast.
			PrivatePCHHeaderFile = "Private/WaveVRPrivatePCH.h";
			var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
			var rendererPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\Renderer\Private");

			PublicIncludePathModuleNames.AddRange(
				new string[]
				{
					"Launch",
					"WaveVRGesture"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"RHI",
					"RenderCore",
					"Renderer",
					"ShaderCore",
					"UtilityShaders",
					"InputCore",
					"HeadMountedDisplay",
					"Slate",
					"SlateCore",
					"OpenGLDrv",
					"ProceduralMeshComponent",
					"WVR",
					"ImageWrapper",
					"UMG",
					"Json",
					"JsonUtilities"
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"AugmentedReality",
				}
			);

			if (Target.bBuildEditor == true)
			{
				PrivateDependencyModuleNames.Add("UnrealEd");
			}

			PublicDefinitions.Add("DEBUG=0");    // Adding your definition here, will add a global Preprocessor value for cpp

			AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenGL");

			if (Target.Platform == UnrealTargetPlatform.Android)
			{
				string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
				AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "WaveVR_UPL.xml"));
				//bUseRTTI = false; //Cause compile fail if true.

				var openglDrvPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\OpenGLDrv\Private");
				var openglPath = Path.Combine(EngineDir, @"Source\ThirdParty\OpenGL");
				PrivateIncludePaths.AddRange(
					new string[] {
						openglDrvPrivatePath,
						openglPath,
						rendererPrivatePath, //***because its not build-in module now*** // "../../../../../Source/Runtime/Renderer/Private",
						// ... add other private include paths required here ...
					}
				);
			}

			if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
			{
				PrivateDependencyModuleNames.AddRange(new string[] {"D3D11RHI", "D3D12RHI"});
				var openglDrvPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\OpenGLDrv\Private");
				var openglPath = Path.Combine(EngineDir, @"Source\ThirdParty\OpenGL");
				PrivateIncludePaths.AddRange(new string[] { openglDrvPrivatePath, openglPath, rendererPrivatePath });
			}
		}
	}
}
