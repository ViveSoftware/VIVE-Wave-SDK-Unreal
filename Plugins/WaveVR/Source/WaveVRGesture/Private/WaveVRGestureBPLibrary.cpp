// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRGestureBPLibrary.h"
#include "FWaveVRGesture.h"
#include "FWaveVRGestureInputDevice.h"

TSharedPtr< class FWaveVRGestureInputDevice > GetGestureDevice()
{
	return FWaveVRGesture::Get().GetInputDevice();
}

void UWaveVRGestureBPLibrary::StartHandGesture()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();

	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
	{
		UE_LOG(LogWaveVRGestureBPLibrary, Log, TEXT("StartHandGesture()"));
		gestureInputDevice->StartHandGesture();
	}
}

void UWaveVRGestureBPLibrary::StopHandGesture()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
	{
		UE_LOG(LogWaveVRGestureBPLibrary, Log, TEXT("StopHandGesture()"));
		gestureInputDevice->StopHandGesture();
	}
}

bool UWaveVRGestureBPLibrary::IsHandGestureAvailable()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
		return gestureInputDevice->IsHandGestureAvailable();
	return false;
}

EWaveVRStaticGesture UWaveVRGestureBPLibrary::GetStaticGestureType(EWaveVRGestureHandType DevType)
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
		return gestureInputDevice->GetStaticGestureType(DevType);
	return EWaveVRStaticGesture::TYPE_INVALID;
}

EWaveVRHandGestureStatus UWaveVRGestureBPLibrary::GetHandGestureStatus()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
		return gestureInputDevice->GetHandGestureStatus();
	return EWaveVRHandGestureStatus::UNSUPPORT;
}

void UWaveVRGestureBPLibrary::StartHandTracking()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
	{
		UE_LOG(LogWaveVRGestureBPLibrary, Log, TEXT("StartHandTracking()"));
		gestureInputDevice->StartHandTracking();
	}
}

void UWaveVRGestureBPLibrary::StopHandTracking()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
	{
		UE_LOG(LogWaveVRGestureBPLibrary, Log, TEXT("StopHandTracking()"));
		gestureInputDevice->StopHandTracking();
	}
}

bool UWaveVRGestureBPLibrary::IsHandTrackingAvailable()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
		return gestureInputDevice->IsHandTrackingAvailable();
	return false;
}

bool UWaveVRGestureBPLibrary::GetBonePositionAndRotation(EWaveVRGestureBoneType BoneId, FVector& OutPosition, FRotator& OutRotation)
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
		return gestureInputDevice->GetBonePositionAndRotation(0, BoneId, OutPosition, OutRotation);
	return false;
}

EWaveVRHandTrackingStatus UWaveVRGestureBPLibrary::GetHandTrackingStatus()
{
	TSharedPtr< class FWaveVRGestureInputDevice > gestureInputDevice = GetGestureDevice();
	if (gestureInputDevice.IsValid() && gestureInputDevice->IsGestureInputDeviceInitialized())
		return gestureInputDevice->GetHandTrackingStatus();
	return EWaveVRHandTrackingStatus::UNSUPPORT;
}
