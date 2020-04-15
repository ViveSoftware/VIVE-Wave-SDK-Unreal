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
#include "WaveVRContentProvider.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API UWaveVRContentProvider : public UObject
{
	GENERATED_BODY()

public:
	UWaveVRContentProvider();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ContentProvider")
	static void OnRoleChange();

private:
#if PLATFORM_ANDROID
	static bool AndroidJavaENVCheck();
	static void AndroidJavaENVFinish();
	static bool mJavaAttached;
#endif

	static UWaveVRContentProvider* mInstance;
};
