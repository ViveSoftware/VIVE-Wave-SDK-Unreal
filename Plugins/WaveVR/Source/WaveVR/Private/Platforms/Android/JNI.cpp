// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#if PLATFORM_ANDROID
#include "RequestResultObject.h"
#include "RequestUsbResultObject.h"
#include "WaveVREventCommon.h"

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <android/log.h>

#undef LOG_TAG
#define LOG_TAG "WaveHMD"
JavaVM * mJavaVM;
JNIEnv * JavaENV = NULL;
jclass mPWclass;
jobject mPWObj;

jclass mRWClazz;
jobject mRWObj;

jclass mJclassCP;
jobject mJobjectCP;

jclass mFUClass;
jobject mFUObject;

jclass mSIPDClass;
jobject mSIPDObject;

jclass mOEMClass;
jobject mOEMObject;

TArray<FString> PermissionArr;
TArray<bool> ResultArr;


extern "C" void Java_com_htc_vr_unreal_PermissionWrapper_initNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Java_com_htc_vr_unreal_PermissionWrapper_initNative");

	JavaENV = LocalJNIEnv;// FAndroidApplication::GetJavaEnv();

	JavaENV->GetJavaVM(&mJavaVM);

	if (mJavaVM == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Java VM is null!!");
		return;
	}

	mPWObj = JavaENV->NewGlobalRef(LocalThiz);

	jclass localClass = JavaENV->FindClass("com/htc/vr/unreal/PermissionWrapper");
	if (localClass == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s: Can't find Java Class - com/htc/vr/unreal/PermissionWrapper.", __func__);
		return;
	}

	mPWclass = reinterpret_cast<jclass>(JavaENV->NewGlobalRef(localClass));
}

extern "C" void Java_com_htc_vr_unreal_PermissionWrapper_requestCallbackNative(JNIEnv* LocalJNIEnv, jobject LocalThiz, jobjectArray PermissionArray, jbooleanArray resultArray) {
	int PermissionSize = LocalJNIEnv->GetArrayLength(PermissionArray);
	int ResultSize = LocalJNIEnv->GetArrayLength(resultArray);
	int ArraySize = (PermissionSize > ResultSize) ? ResultSize : PermissionSize;

	PermissionArr.Empty();
	ResultArr.Empty();
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "------ ArraySize = %d", ArraySize);
	jboolean* javaBarray = LocalJNIEnv->GetBooleanArrayElements(resultArray, 0);

	for (int i = 0; i < ArraySize; i++)
	{
		jstring string = (jstring)LocalJNIEnv->GetObjectArrayElement(PermissionArray, i);
		jboolean boolr = javaBarray[i];
		const char* myarray = LocalJNIEnv->GetStringUTFChars(string, 0);
		FString tmp = FString(UTF8_TO_TCHAR(myarray));
		PermissionArr.Add(tmp);
		__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Permission = %s", TCHAR_TO_ANSI(*PermissionArr[i]));
		ResultArr.Add(boolr);
		__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Result = %d", ResultArr[i]);
		LocalJNIEnv->ReleaseStringUTFChars(string, myarray);
		LocalJNIEnv->DeleteLocalRef(string);
	}
	LocalJNIEnv->DeleteLocalRef(LocalThiz);
	LocalJNIEnv->DeleteLocalRef(PermissionArray);
	LocalJNIEnv->DeleteLocalRef(resultArray);
	URequestResultObject::onRequestResultNative.Broadcast(PermissionArr, ResultArr);
}

extern "C" void Java_com_htc_vr_unreal_PermissionWrapper_requestUsbCallbackNative(JNIEnv* LocalJNIEnv, jobject LocalThiz,jboolean result) {
       URequestUsbResultObject::onRequestUsbResultNative.Broadcast((bool)result);
}


extern "C" void Java_com_htc_vr_unreal_ResourceWrapper_initNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Java_com_htc_vr_unreal_ResourceWrapper_initNative");

	JavaENV = LocalJNIEnv;// FAndroidApplication::GetJavaEnv();

	JavaENV->GetJavaVM(&mJavaVM);

	if (mJavaVM == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Java VM is null!!");
		return;
	}

	mRWObj = JavaENV->NewGlobalRef(LocalThiz);

	jclass localClass = JavaENV->FindClass("com/htc/vr/unreal/ResourceWrapper");
	if (localClass == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s: Can't find Java Class - com/htc/vr/unreal/PermissionManager.", __func__);
		return;
	}

	mRWClazz = reinterpret_cast<jclass>(JavaENV->NewGlobalRef(localClass));
}

extern "C" void Java_com_htc_vr_unreal_ContentProvider_initNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Java_com_htc_vr_unreal_ContentProvider_initNative");

	JavaENV = LocalJNIEnv;// FAndroidApplication::GetJavaEnv();

	JavaENV->GetJavaVM(&mJavaVM);

	if (mJavaVM == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Java VM is null!!");
		return;
	}

	mJobjectCP = JavaENV->NewGlobalRef(LocalThiz);

	jclass localClass = JavaENV->FindClass("com/htc/vr/unreal/ContentProvider");
	if (localClass == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s: Can't find Java Class - com/htc/vr/unreal/ContentProvider.", __func__);
		return;
	}

	mJclassCP = reinterpret_cast<jclass>(JavaENV->NewGlobalRef(localClass));
}

extern "C" void Java_com_htc_vr_unreal_FileUtils_initNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Java_com_htc_vr_unreal_FileUtils_initNative");

	JavaENV = LocalJNIEnv;// FAndroidApplication::GetJavaEnv();

	JavaENV->GetJavaVM(&mJavaVM);

	if (mJavaVM == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Java VM is null!!");
		return;
	}

	mFUObject = JavaENV->NewGlobalRef(LocalThiz);

	jclass localClass = JavaENV->FindClass("com/htc/vr/unreal/FileUtils");
	if (localClass == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s: Can't find Java Class - com/htc/vr/unreal/FileUtils.", __func__);
		return;
	}

	mFUClass = reinterpret_cast<jclass>(JavaENV->NewGlobalRef(localClass));
}

extern "C" void Java_com_htc_vr_unreal_SoftwareIpd_initNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	//LOG_FUNC();
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Java_com_htc_vr_unreal_SoftwareIpd_initNative");

	JavaENV = LocalJNIEnv;// FAndroidApplication::GetJavaEnv();

	JavaENV->GetJavaVM(&mJavaVM);

	if (mJavaVM == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Java VM is null!!");
		return;
	}

	mSIPDObject = JavaENV->NewGlobalRef(LocalThiz);

	jclass localClass = JavaENV->FindClass("com/htc/vr/unreal/SoftwareIpd");
	if (localClass == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s: Can't find Java Class - com/htc/vr/unreal/SoftwareIpd.", __func__);
		return;
	}

	mSIPDClass =  reinterpret_cast<jclass>(JavaENV->NewGlobalRef(localClass));
}

extern "C" void Java_com_htc_vr_unreal_OEMConfig_initNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Java_com_htc_vr_unreal_OEMConfig_initNative");

	JavaENV = LocalJNIEnv;// FAndroidApplication::GetJavaEnv();

	JavaENV->GetJavaVM(&mJavaVM);

	if (mJavaVM == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Java VM is null!!");
		return;
	}

	mOEMObject = JavaENV->NewGlobalRef(LocalThiz);

	jclass localClass = JavaENV->FindClass("com/htc/vr/unreal/OEMConfig");
	if (localClass == nullptr) {
		__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "%s: Can't find Java Class - com/htc/vr/unreal/OEMConfig.", __func__);
		return;
	}

	mOEMClass = reinterpret_cast<jclass>(JavaENV->NewGlobalRef(localClass));
}

extern "C" void Java_com_htc_vr_unreal_OEMConfig_ConfigChangedNative(JNIEnv* LocalJNIEnv, jobject LocalThiz) {
	__android_log_print(ANDROID_LOG_INFO, LOG_TAG, "ConfigChangedNative");

	UWaveVREventCommon::OnOEMConfigChangeNative.Broadcast();
}

#endif  // PLATFORM_ANDROID
