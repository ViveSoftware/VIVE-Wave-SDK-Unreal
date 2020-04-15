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

#if WITH_EDITOR

#define PLATFORM_CHAR(str) str
// fallback to UE_LOG
#define LOGV(CategoryName, Format, ...) UE_LOG(CategoryName, Verbose, TEXT(Format), ##__VA_ARGS__)
#define LOGD(CategoryName, Format, ...) UE_LOG(CategoryName, Display, TEXT(Format), ##__VA_ARGS__)
#define LOGI(CategoryName, Format, ...) UE_LOG(CategoryName, Display, TEXT(Format), ##__VA_ARGS__)
#define LOGW(CategoryName, Format, ...) UE_LOG(CategoryName, Warning, TEXT(Format), ##__VA_ARGS__)
#define LOGE(CategoryName, Format, ...) UE_LOG(CategoryName, Error, TEXT(Format), ##__VA_ARGS__)
#define USEWIDE(str) *FString(str)  // Performance may not good.

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
