// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WaveVRItemActions.generated.h"

/**
 *
 */
UCLASS()
class WAVEVREDITOR_API UWaveVRItemActions : public UObject
{
	GENERATED_BODY()

public:
	static void StartDPServer();
	static void StopDPServer();
	static void InstallDeviceAPK();
	static void StartDeviceAPK();
	static void StopDeviceAPK();

private:
	static FString GetLocalIPAddr();
	static void WriteConfig(FString configFile);
	static void CheckConfigAndDelete(FString configFile);
	static void StopDirectPreviewProcessInner();
};
