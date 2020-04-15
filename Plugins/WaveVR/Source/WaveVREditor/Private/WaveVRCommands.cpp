// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRCommands.h"

#define LOCTEXT_NAMESPACE "FWaveVRModule"

void FWaveVRCommands::RegisterCommands()
{
	//UI_COMMAND(PluginAction, "Example", "Tooltips", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StartDPServer, "Start DPServer", "Start DPServer", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StopDPServer, "Stop DPServer", "Stop DPServer", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(InstallDeviceAPK, "Install Device APK", "Install Direct Preview APK to HMD and configure (adb is need)", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StartDeviceAPK, "Start Device APK", "Start Direct Preview APK (adb is need)", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(StopDeviceAPK, "Stop Device APK", "Stop Direct Preview APK (adb is need)", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
