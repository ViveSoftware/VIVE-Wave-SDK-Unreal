// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

/**
* Created by devinyu on 2017/9/27.
*/

#pragma once

#include "CoreMinimal.h"
#include "RequestResultObject.h"
#include "RequestUsbResultObject.h"
#include "UObject/NoExportTypes.h"
#include "WaveVRPermissionManager.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API UWaveVRPermissionManager : public UObject
{
	GENERATED_BODY()

public:
	UWaveVRPermissionManager();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission", meta = (
		ToolTip = "Return true if permission dialog will show on VR scene."))
	static bool showDialogOnVRScene();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission", meta = (
		ToolTip = "Return true if this permission is already granted."))
	static bool isPermissionGranted(FString permission);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission", meta = (
		ToolTip = "Return true if this permission should be granted."))
	static bool shouldGrantPermission(FString permission);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission", meta = (
		ToolTip = "Show a dialog to let user choice grant or deny for permissions you want to request."))
	static URequestResultObject* requestPermissions(TArray<FString> permissions);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission", meta = (
		ToolTip = "Show a dialog to let user choice grant or deny for Usb permissions you want to request."))
	static URequestUsbResultObject* requestUsbPermission();

private:
	//bool isProcessing;
	static URequestResultObject* mResultObject;
	static URequestUsbResultObject* mUsbResultObject;
};
