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

#include "WaveVRPermissionManager.h"
#include "WaveVRPrivatePCH.h"
#include "Logging/LogMacros.h"

#include <vector>
#include <string>

DEFINE_LOG_CATEGORY_STATIC(WVR_PermissionMgr, Display, All);

URequestResultObject* UWaveVRPermissionManager::mResultObject = NULL;
URequestUsbResultObject* UWaveVRPermissionManager::mUsbResultObject = NULL;

#pragma region Class UWaveVRPermissionManager
UWaveVRPermissionManager::UWaveVRPermissionManager()
{
	LOGI(WVR_PermissionMgr, "constructor");
	//isProcessing = false;
	mResultObject = NewObject<URequestResultObject>(URequestResultObject::StaticClass());
	mUsbResultObject = NewObject<URequestUsbResultObject>(URequestUsbResultObject::StaticClass());
}

URequestResultObject* UWaveVRPermissionManager::requestPermissions(TArray<FString> permissions)
{
	LOGI(WVR_PermissionMgr, "requestPermissions");
	mResultObject->setResult(false);
	std::vector<std::string> permissionArray;

	int len = permissions.Num();
	for (int i = 0; i < len; i++)
	{
		std::string StdString(TCHAR_TO_ANSI(*permissions[i]));
		permissionArray.push_back(StdString);
	}

	mResultObject->receiveSignal();
	bool ret = WVR()->RequestPermissions(permissionArray);
	LOGI(WVR_PermissionMgr, "RequestPermissions native ret = %d", ret);
	mResultObject->setResult(ret);

	return mResultObject;
}

URequestUsbResultObject* UWaveVRPermissionManager::requestUsbPermission()
{
	LOGI(WVR_PermissionMgr, "requestUsbPermission");
	mUsbResultObject->setResult(false);
	mUsbResultObject->receiveSignal();
	bool ret = WVR()->RequestUsbPermission();
	LOGI(WVR_PermissionMgr, "RequestUsbPermission native ret = %d", ret);
	mUsbResultObject->setResult(ret);
	return mUsbResultObject;
}

bool UWaveVRPermissionManager::isPermissionGranted(FString permission)
{
	LOGI(WVR_PermissionMgr, "isPermissionGranted, permission = %s", PLATFORM_CHAR(*permission));
	std::string StdString(TCHAR_TO_ANSI(*permission));
	bool ret = WVR()->IsPermissionGranted(StdString);
	LOGI(WVR_PermissionMgr, "IsPermissionGranted native permission = %s, ret = %d", StdString.c_str(), ret);
	return ret;
}

bool UWaveVRPermissionManager::shouldGrantPermission(FString permission)
{
	LOGI(WVR_PermissionMgr, "shouldGrantPermission, permission = %s", PLATFORM_CHAR(*permission));
	std::string StdString(TCHAR_TO_ANSI(*permission));
	bool ret = WVR()->ShouldPermissionGranted(StdString);
	LOGI(WVR_PermissionMgr, "ShouldPermissionGranted native permission = %s, ret = %d", StdString.c_str(), ret);
	return ret;
}

bool UWaveVRPermissionManager::showDialogOnVRScene() {
	LOGI(WVR_PermissionMgr, "showDialogOnVRScene");
	bool ret = WVR()->ShowDialogOnVRScene();
	LOGI(WVR_PermissionMgr, "ShowDialogOnVRScene native ret = %d", ret);
	return ret;
}
#pragma endregion
