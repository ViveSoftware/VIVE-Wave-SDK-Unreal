// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRContentProvider.h"
#include "WaveVRPrivatePCH.h"
#include "WaveVREventCommon.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(WaveVRContentProvider, Display, All);

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"

extern JavaVM* mJavaVM;
extern JNIEnv* JavaENV;
extern jclass mJclassCP;
extern jobject mJobjectCP;
#endif

UWaveVRContentProvider* UWaveVRContentProvider::mInstance;
#if PLATFORM_ANDROID
bool UWaveVRContentProvider::mJavaAttached(false);
#endif

UWaveVRContentProvider::UWaveVRContentProvider()
{
	LOGI(WaveVRContentProvider, "UWaveVRContentProvider()");
}

void UWaveVRContentProvider::OnRoleChange()
{
	bool _lefthanded = UWaveVREventCommon::IsLeftHandedMode();
	FString _role = _lefthanded ? "2" : "1";

#if PLATFORM_ANDROID
	LOGI(WaveVRContentProvider, "OnRoleChange() left-handed? %d", _lefthanded);

	if (!AndroidJavaENVCheck()) {
		LOGE(WaveVRContentProvider, "OnRoleChange() AndroidJavaENVCheck failsed.");
		return;
	}

	jmethodID _JID_WriteControllerRole = JavaENV->GetMethodID(mJclassCP, "writeControllerRoleValue", "(Ljava/lang/String;)V");

	if (!_JID_WriteControllerRole)
	{
		LOGE(WaveVRContentProvider, "OnRoleChange() method writeControllerRoleValue is not found.");
		if (JavaENV->ExceptionCheck())
		{
			JavaENV->ExceptionClear();
		}
		return;
	}

	jstring _JSTR_role = JavaENV->NewStringUTF(TCHAR_TO_ANSI(*_role));
	JavaENV->CallVoidMethod(mJobjectCP, _JID_WriteControllerRole, _JSTR_role);
	JavaENV->DeleteLocalRef(_JSTR_role);
#endif
}

#if PLATFORM_ANDROID
bool UWaveVRContentProvider::AndroidJavaENVCheck() {
	if (!mJclassCP || !mJobjectCP || !JavaENV || !mJavaVM) {
		LOGE(WaveVRContentProvider, "Cant find Java class/method.");
		return false;
	}

	mJavaAttached = false;

	switch (mJavaVM->GetEnv((void**)&JavaENV, JNI_VERSION_1_6)) {
	case JNI_OK:
		//LOGD(WaveVRContentProvider, "GetEnv is JNI_OK");
		break;
	case JNI_EDETACHED:
		//LOGD(WaveVRContentProvider, "GetEnv is JNI_EDETACHED");
		if (mJavaVM->AttachCurrentThread(&JavaENV, nullptr) != JNI_OK) {
			LOGE(WaveVRContentProvider, "Failed to attach current thread");
			return false;
		}
		mJavaAttached = true;
		break;
	case JNI_EVERSION:
		//LOGD(WaveVRContentProvider, "GetEnv is JNI_EVERSION");
		break;
	}

	return true;
}

void UWaveVRContentProvider::AndroidJavaENVFinish() {
	if (mJavaAttached) {
		mJavaVM->DetachCurrentThread();
	}
}

#endif
