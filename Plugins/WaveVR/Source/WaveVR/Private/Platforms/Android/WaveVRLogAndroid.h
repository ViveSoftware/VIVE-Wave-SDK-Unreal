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

#if PLATFORM_ANDROID
#define PLATFORM_CHAR(str) TCHAR_TO_UTF8(str)

#include <android/Log.h>
#define LOGV(TAG, fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, #TAG, fmt, ##__VA_ARGS__)
#define LOGD(TAG, fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, #TAG, fmt, ##__VA_ARGS__)
#define LOGI(TAG, fmt, ...) __android_log_print(ANDROID_LOG_INFO, #TAG, fmt, ##__VA_ARGS__)
#define LOGW(TAG, fmt, ...) __android_log_print(ANDROID_LOG_WARN, #TAG, fmt, ##__VA_ARGS__)
#define LOGE(TAG, fmt, ...) __android_log_print(ANDROID_LOG_ERROR, #TAG, fmt, ##__VA_ARGS__)
#define USEWIDE(str) str

// Warning! If enable All log, some log will be missed.
#define WAVEVR_LOG_SHOW_ALL_ENTRY  0
#define WAVEVR_LOG_ENTRY_LIFECYCLE (WAVEVR_LOG_SHOW_ALL_ENTRY || 0)

#ifndef __FUNCTION__
// The g++ __func__ is too long to read.
#define WVR_FUNCTION_STRING USEWIDE(__func__)
#else
#define WVR_FUNCTION_STRING USEWIDE(__FUNCTION__)
#endif

#if WAVEVR_LOG_SHOW_ALL_ENTRY
#define LOG_FUNC() LOGD(LogWaveVRHMD, "%s", WVR_FUNCTION_STRING);
#else
#define LOG_FUNC()
#endif

#define LOG_FUNC_IF(expr) do { constexpr decltype(expr) var = (expr); if (var) { LOGD(WVRHMD, "%s", WVR_FUNCTION_STRING); } } while (0)

#endif


