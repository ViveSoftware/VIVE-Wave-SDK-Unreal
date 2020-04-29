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
using UnrealBuildTool;

public class WVR : ModuleRules
{
	public WVR(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		string WVRSDKInc = Target.UEThirdPartySourceDirectory + "WVR/include/wvr";
		string AssimpInc = Target.UEThirdPartySourceDirectory + "WVR/include";
		string WVRSDKLib = Target.UEThirdPartySourceDirectory + "WVR/lib";
		string WVRSimulator = Target.UEThirdPartySourceDirectory + "WVR";

		PublicSystemIncludePaths.AddRange(
			new string[] {
				WVRSDKInc,
				AssimpInc,
				WVRSimulator
			}
		);

		if(Target.Platform == UnrealTargetPlatform.Android) {
			string[] SharedLibs = {
			    "libwvr_api.so",
			    "libwvr_internal_using_assimp.so",
			};
			string[] Archs = {
			    "armeabi-v7a",
			    "arm64-v8a",
			};
			foreach (string so in SharedLibs)
			{
				foreach (string arch in Archs)
				{
					PublicAdditionalLibraries.Add(Path.Combine(WVRSDKLib, "android", arch, so));
				}
			}
		} else if (Target.bBuildEditor == true) {
			PublicDelayLoadDLLs.Add("WaveVR_Simulator.dll");
			RuntimeDependencies.Add(WVRSDKLib + "/Win64/WaveVR_Simulator.dll");
		} else if (Target.Platform == UnrealTargetPlatform.Win64){
			PublicDelayLoadDLLs.Add("wave_api.dll");
			RuntimeDependencies.Add(WVRSDKLib + "/Win64/wave_api.dll");
		}
	}
}
