// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Interfaces/IPluginManager.h"
#include "WaveVRItemActions.h"
#include <SocketSubsystem.h>
#include <IPAddress.h>
#include "PlatformFilemanager.h"
#include "GenericPlatformFile.h"

DEFINE_LOG_CATEGORY_STATIC(WVRMenuItemAction, Display, All);

void UWaveVRItemActions::StartDPServer() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("startDPServer"));

	//UE_LOG(WVRMenuItemAction, Display, TEXT("Working directory is %s"), *(FPlatformProcess::GetCurrentWorkingDirectory()));
	FString workingDir = IPluginManager::Get().FindPlugin("WaveVR")->GetBaseDir();
	UE_LOG(WVRMenuItemAction, Display, TEXT("Plugin directory is %s"), *workingDir);
	FString execCmd = TEXT("/c cd ") + workingDir + TEXT("/Prebuilt/DirectPreview/dpServer") + TEXT(" && dpServer");
	UE_LOG(WVRMenuItemAction, Display, TEXT("start dpServer: %s"), *execCmd);
	FProcHandle a = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *execCmd, true, false, false, nullptr, 0, nullptr, nullptr);
}

void UWaveVRItemActions::StopDPServer() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("StopDPServer"));

	FString execCmd = TEXT("/c taskkill /F /IM dpServer.exe");
	UE_LOG(WVRMenuItemAction, Display, TEXT("stop dpServer: %s"), *execCmd);
	FProcHandle a = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *execCmd, true, false, false, nullptr, 0, nullptr, nullptr);
}

void UWaveVRItemActions::InstallDeviceAPK() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("InstallDeviceAPK"));
	FString pluginDir = IPluginManager::Get().FindPlugin("WaveVR")->GetBaseDir();
	FString configDir = pluginDir + TEXT("/Prebuilt/DirectPreview/deviceAPK/");
	FString configFile = pluginDir + TEXT("/Prebuilt/DirectPreview/deviceAPK/DirectPreviewConfig.json");

	FString installCmd = TEXT("/c cd ") + configDir + TEXT(" && adb install -r -d wvr_plugins_directpreview_agent_unreal.apk");
	UE_LOG(WVRMenuItemAction, Display, TEXT("install apk: %s"), *installCmd);
	FProcHandle pInstall = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *installCmd, true, false, false, nullptr, 0, nullptr, nullptr);

	WriteConfig(configFile);

	FProcHandle pMakeDir = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell mkdir /sdcard/DirectPreview/"), true, false, false, nullptr, 0, nullptr, nullptr);

	FString execCmd = TEXT("/c cd ") + configDir + TEXT(" && adb push DirectPreviewConfig.json /sdcard/DirectPreview/config.json");
	UE_LOG(WVRMenuItemAction, Display, TEXT("push config: %s"), *execCmd);
	FProcHandle pPushConfig = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), *execCmd, true, false, false, nullptr, 0, nullptr, nullptr);
}

void UWaveVRItemActions::StartDeviceAPK() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("StartDeviceAPK"));
	FProcHandle pStartAPK = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell am start -n com.htc.vr.directpreview.agent.unreal/com.vive.rrclient.RRClient"), true, false, false, nullptr, 0, nullptr, nullptr);
}

void UWaveVRItemActions::StopDeviceAPK() {
	UE_LOG(WVRMenuItemAction, Display, TEXT("StopDeviceAPK"));

	StopDirectPreviewProcessInner();
}

void UWaveVRItemActions::StopDirectPreviewProcessInner() {
	FProcHandle pStopAPK = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell am force-stop com.htc.vr.directpreview.agent.unreal"), true, false, false, nullptr, 0, nullptr, nullptr);

	FProcHandle pKillProcess = FPlatformProcess::CreateProc(TEXT("C:\\WINDOWS\\system32\\cmd.exe"), TEXT("/c adb shell am kill com.htc.vr.directpreview.agent.unreal"), true, false, false, nullptr, 0, nullptr, nullptr);
}

FString UWaveVRItemActions::GetLocalIPAddr() {
	bool canBind = false;
	TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
	return localIp->IsValid() ? localIp->ToString(false) : TEXT("");
}

void UWaveVRItemActions::WriteConfig(FString configFile) {
	UE_LOG(WVRMenuItemAction, Display, TEXT("WriteConfig"));
	CheckConfigAndDelete(configFile);

	FString localIP = GetLocalIPAddr();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* FileHandle = PlatformFile.OpenWrite(*configFile);

	if (FileHandle) {
		FString cn = FString(
			TEXT("{\n")
			TEXT(" \"IP\" : \"") + localIP + TEXT("\", \n")
			TEXT(" \"Port\" : 6555,\n")
			TEXT(" \"HMD\" : \"FOCUS\",\n")
			TEXT(" \n")
			TEXT(" \"RenderWidth\" : 1440,\n")
			TEXT(" \"RenderHeight\" : 1600,\n")
			TEXT(" \"RenderSizeScale\" : 1.0,\n")
			TEXT(" \"RenderOverfillScale\" : 1.3,\n")
			TEXT(" \n")
			TEXT(" \"UseAutoPrecdictTime\" : true,\n")
			TEXT(" \"CtlPredictRate\" : 6,\n")
			TEXT(" \"HmdPredictRatio\" : 0.615,\n")
			TEXT(" \"CtlPredictRatio\" : 0.615,\n")
			TEXT(" \"HmdPredict\" : 41,\n")
			TEXT(" \"ControllerPredict\" : 40,\n")
			TEXT(" \"MaxHmdPredictTimeInMs\" : 35,\n")
			TEXT(" \"MaxCtlPredictTimeInMs\" : 20,\n")
			TEXT(" \n")
			TEXT(" \"RoomHeight\" : 1.6\n")
			TEXT("}\n")
		);
		FileHandle->Write((const uint8*)TCHAR_TO_ANSI(*cn), cn.Len());
		delete FileHandle;
	}
}

void UWaveVRItemActions::CheckConfigAndDelete(FString configFile) {
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*configFile)) {
		PlatformFile.DeleteFile(*configFile);
	}
}
