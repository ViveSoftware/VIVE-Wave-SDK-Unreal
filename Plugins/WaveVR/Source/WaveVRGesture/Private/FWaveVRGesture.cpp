// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "FWaveVRGesture.h"

void FWaveVRGesture::StartupModule()
{
	IInputDeviceModule::StartupModule();
	UE_LOG(LogWaveVRGesture, Log, TEXT("StartupModule"));
}


void FWaveVRGesture::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

TSharedPtr< class IInputDevice > FWaveVRGesture::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	gestureInputDevice = MakeShareable(new FWaveVRGestureInputDevice(InMessageHandler));

	UE_LOG(LogWaveVRGesture, Log, TEXT("CreateInputDevice() FWaveVRGestureInputDevice"));
	return TSharedPtr< class IInputDevice >(gestureInputDevice);
}

IMPLEMENT_MODULE(FWaveVRGesture, WaveVRGesture)
