// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "SoftwareIpd.h"
#include "WaveVRPrivatePCH.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include <android/log.h>

#define SIPD_TAG "UASoftwareIpd_tags"
extern JavaVM* mJavaVM;
extern JNIEnv* JavaENV;

extern jclass mSIPDClass;
extern jobject mSIPDObject;

#endif


USoftwareIpd::USoftwareIpd() {
}

bool USoftwareIpd::do_read_ipd(FString &mComponentName, FString &isEnable, FString &value)
{

#if PLATFORM_ANDROID
	jmethodID methodid = JavaENV->GetMethodID(mSIPDClass, "read_ipd", "()[Ljava/lang/String;");
	if (!methodid)
	{
		LOGE(SIPD_TAG, "ERROR: read_ipd Method cant be found Oz");
		return false;
	}

	jobjectArray data = (jobjectArray)JavaENV->CallObjectMethod(mSIPDObject, methodid);
	jsize const length = JavaENV->GetArrayLength(data);

	__android_log_print(ANDROID_LOG_INFO, SIPD_TAG, "read_ipd Get string array length : %d.", length);

	const char *param[length];

	for (int idx = 0; idx < length; ++idx) {

		jstring string = (jstring)JavaENV->GetObjectArrayElement(data, idx);
		param[idx] = JavaENV->GetStringUTFChars(string, 0);

		LOGI(SIPD_TAG, "read_ipd Get string idx: <%d> , word: %s.", idx, param[idx]);

		if (idx == 0) {
			mComponentName = param[idx];
		}
		else if (idx == 1) {
			isEnable = param[idx];
		}
		else if (idx == 2) {
			value = param[idx];
		}

		JavaENV->ReleaseStringUTFChars(string, param[idx]);
		JavaENV->DeleteLocalRef(string);
	}

#endif
	return true;
}

bool USoftwareIpd::do_write_ipd(EWVR_SoftwareIpd_value value, bool isEnable)
{

#if PLATFORM_ANDROID
	//float w_value = 0.0f;
	jstring s_value = JavaENV->NewStringUTF("0.0");

	if (value == EWVR_SoftwareIpd_value::EWVR_IPD_VALUE_56) {
		//w_value = 0.056f;
		s_value = JavaENV->NewStringUTF("0.056");
	}
	else if (value == EWVR_SoftwareIpd_value::EWVR_IPD_VALUE_63) {
		//w_value = 0.063f;
		s_value = JavaENV->NewStringUTF("0.063");
	}
	else if (value == EWVR_SoftwareIpd_value::EWVR_IPD_VALUE_70) {
		//w_value = 0.070f;
		s_value = JavaENV->NewStringUTF("0.070");
	}
	else if (value == EWVR_SoftwareIpd_value::EWVR_IPD_VALUE_ZERO) {
		//w_value = 0.070f;
		s_value = JavaENV->NewStringUTF("0.000");
	}

	else {
		;;
		//w_value = 0.0f;
	}

	jmethodID methodid = JavaENV->GetMethodID(mSIPDClass, "write_ipd", "(Ljava/lang/String;Ljava/lang/String;)Z");
	if (!methodid)  {
		LOGE(SIPD_TAG, "ERROR: write_ipd Method cant be found.");
		return false;
	}

	jstring s_isEnable;
	if (isEnable == true) {
		s_isEnable = JavaENV->NewStringUTF("true");
		LOGI(SIPD_TAG, "write_ipd Method isEnabled is true.");
	} else {
		s_isEnable = JavaENV->NewStringUTF("false");
		LOGI(SIPD_TAG, "write_ipd Method isEnabled is false.");
	}

	jboolean ret_b = JavaENV->CallBooleanMethod(mSIPDObject, methodid, s_value, s_isEnable);
	if (ret_b != true)  {
		LOGE(SIPD_TAG, "ERROR: write_ipd is false.");
		return false;
	}
#endif
	return true;
}
