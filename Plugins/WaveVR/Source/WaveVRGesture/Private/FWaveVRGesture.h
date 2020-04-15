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
#include "IWaveVRGesture.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRGesture, Log, All);

class FWaveVRGesture : public IWaveVRGesture
{
public:
	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler);

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice;
	virtual TSharedPtr< class FWaveVRGestureInputDevice > GetInputDevice() { return gestureInputDevice; }

private:
	TSharedPtr< class FWaveVRGestureDeviceManager > DeviceManager;
};
