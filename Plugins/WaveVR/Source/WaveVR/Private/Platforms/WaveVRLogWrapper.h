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

#include "Logging/LogMacros.h"

#if WITH_EDITOR
	#include "Editor/WaveVRLogEditor.h"
#else
    #if PLATFORM_ANDROID
        #include "Android/WaveVRLogAndroid.h"
    #elif PLATFORM_WINDOWS
        #include "Windows/WaveVRLogWindows.h"
    #else
        #define LOGV(TAG, fmt, ...)
        #define LOGD(TAG, fmt, ...)
        #define LOGI(TAG, fmt, ...)
        #define LOGW(TAG, fmt, ...)
        #define LOGE(TAG, fmt, ...)
        #define USEWIDE(str) str
        #define LOG_FUNC()
    #endif
#endif
