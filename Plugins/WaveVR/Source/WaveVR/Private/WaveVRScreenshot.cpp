// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRScreenshot.h"
#include "WaveVRPrivatePCH.h"

#include <ctime>

#if WAVEVR_SUPPORTED_PLATFORMS
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif

DEFINE_LOG_CATEGORY(LogWaveVRScreenshot);

// class ScreenshotImpl
ScreenshotImpl* ScreenshotImpl::mInstance = nullptr;

ScreenshotImpl::ScreenshotImpl() {
}

ScreenshotImpl* ScreenshotImpl::GetInstance() {

	if (mInstance == nullptr) {
		mInstance = new ScreenshotImpl();
	}

	return mInstance;
}

bool ScreenshotImpl::Screenshot(EScreenshotMode ScreenShotMode) {

	unsigned int width = 0;
	unsigned int height = 0;

	struct tm ltm;
	time_t nT = time(0);

	char nowTime[20];
	char fileName[50];

#if	WAVEVR_SUPPORTED_PLATFORMS
	localtime_r(&nT, &ltm);
#else
	localtime_s(&ltm, &nT);
#endif

	FWaveVRAPIWrapper::GetInstance()->GetRenderTargetSize(&width, &height);
	WVR_ScreenshotMode screenMode = WVR_ScreenshotMode_Default;

	switch (ScreenShotMode)
	{
	case EScreenshotMode::DEFAULT:
		screenMode = WVR_ScreenshotMode_Default;
		LOGI(LogWaveVRScreenshot, "Oz, Screenshot Mode: Default.");
		break;
	case EScreenshotMode::RAW:
		screenMode = WVR_ScreenshotMode_Raw;
		LOGI(LogWaveVRScreenshot, "Oz, Screenshot Mode: Raw.");
		break;
	case EScreenshotMode::DISTORTED:
		screenMode = WVR_ScreenshotMode_Distorted;
		LOGI(LogWaveVRScreenshot, "Oz, Screenshot Mode: Distorted.");
		break;
	default:
		break;
	}

#if	WAVEVR_SUPPORTED_PLATFORMS
	snprintf(nowTime, sizeof(ltm), "%d%02d-%d%d%d%d",
		ltm.tm_year + 1900,
		ltm.tm_mon + 1,
		ltm.tm_mday,
		ltm.tm_hour,
		ltm.tm_min,
		ltm.tm_sec);
	snprintf(fileName, sizeof(fileName), "%s_%s", "ScreenTest", nowTime);
#else
	sprintf_s(nowTime, sizeof(ltm), "%d%02d-%d%d%d%d",
		ltm.tm_year + 1900,
		ltm.tm_mon + 1,
		ltm.tm_mday,
		ltm.tm_hour,
		ltm.tm_min,
		ltm.tm_sec);
	sprintf_s(fileName, sizeof(fileName), "%s_%s", "ScreenTest", nowTime);
#endif
	//Update Screenshot file name
	ScreenshotFileName = fileName;

	LOGI(LogWaveVRScreenshot, "Oz Screenshot File Name: %s", TCHAR_TO_ANSI(*ScreenshotFileName));
	return FWaveVRAPIWrapper::GetInstance()->RequestScreenshot(width, height, screenMode, fileName);
}
