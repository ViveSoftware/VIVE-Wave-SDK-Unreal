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

#include "CoreMinimal.h"
#include "WaveVRGestureEnums.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WaveVRGestureBPLibrary.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRGestureBPLibrary, Log, All);

UCLASS()
class WAVEVRGESTURE_API UWaveVRGestureBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "To enable the Hand Gesture component."))
	static void StartHandGesture();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "To disable the Hand Gesture component."))
	static void StopHandGesture();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "To check if the Hand Gesture component is available."))
	static bool IsHandGestureAvailable();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "If the Hand Gesture component is available, this API is used for getting the static gesture type."))
	static EWaveVRStaticGesture GetStaticGestureType(EWaveVRGestureHandType DevType);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "Check current hand gesture status."))
	static EWaveVRHandGestureStatus GetHandGestureStatus();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "To enable the Hand Tracking component."))
	static void StartHandTracking();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "To disable the Hand Tracking component."))
	static void StopHandTracking();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "To check if the Hand Tracking component is available."))
	static bool IsHandTrackingAvailable();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "If the Hand Tracking component is available, this API is used for getting the bone poses."))
	static bool GetBonePositionAndRotation(EWaveVRGestureBoneType BoneId, FVector& OutPosition, FRotator& OutRotation);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Gesture",
		meta = (ToolTip = "Check current hand tracking status."))
	static EWaveVRHandTrackingStatus GetHandTrackingStatus();
};
