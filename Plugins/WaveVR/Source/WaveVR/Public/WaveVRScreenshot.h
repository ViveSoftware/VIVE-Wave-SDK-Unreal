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
#include "WaveVRBlueprintFunctionLibrary.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWaveVRScreenshot, Log, All);

//Screenshot class
class ScreenshotImpl {

public:
	///Singleton Pattern
	ScreenshotImpl();

	static ScreenshotImpl* GetInstance();
	bool Screenshot(EScreenshotMode ScreenShotMode);
	FString ScreenshotFileName;
	const FString ScreenshotImagePath = "/sdcard/Pictures/Screenshots/";

private:
	static ScreenshotImpl* mInstance;
};
