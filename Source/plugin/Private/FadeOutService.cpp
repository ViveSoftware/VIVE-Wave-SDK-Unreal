// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "FadeOutService.h"

#if PLATFORM_ANDROID

// Get a reference to the JNI environment
//#include "Android/AndroidApplication.h"
// Get a reference to the JVM
//#include "Android/AndroidJNI.h"

#include "AndroidPlatform.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidJava.h"

#include <android/log.h>
#include <android_native_app_glue.h>
#include <jni.h>

#endif

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRFadeOut, Log, All);

void UFadeOutService::UFadeOut()
{
#if PLATFORM_ANDROID

	UE_LOG(LogWaveVRFadeOut, Log, TEXT("UFadeOut entry."));
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		UE_LOG(LogWaveVRFadeOut, Log, TEXT("UFadeOut GetJavaEnv."));
		jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getApplicationContext", "()Landroid/content/Context;", false);
		jobject Context = FJavaWrapper::CallObjectMethod(Env, FAndroidApplication::GetGameActivityThis(), Method);
		jmethodID MethodmVR = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getVRInstance", "()Lcom/htc/vr/sdk/VR;", false);
		jobject mVR = FJavaWrapper::CallObjectMethod(Env, FAndroidApplication::GetGameActivityThis(), MethodmVR);
		jclass classActivity = FAndroidApplication::FindJavaClass("com/htc/vr/unreal/FadeOutStaticActivity");
		jmethodID onFadeOutMethod = FJavaWrapper::FindStaticMethod(Env, classActivity, "onFadeOutStaticActivity", "(Landroid/content/Context;Lcom/htc/vr/sdk/VR;)V", true);
		Env->CallStaticVoidMethod(classActivity, onFadeOutMethod, Context, mVR);
	}
	else
	{
		UE_LOG(LogWaveVRFadeOut, Warning, TEXT("ERROR: Could not get Java ENV\n"));
	}

	UE_LOG(LogWaveVRFadeOut, Log, TEXT("UFadeOut exit."));
#endif
}
