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
#include "Platforms/WaveVRAPIWrapper.h"

class FFramePoses
{
public:
	FFramePoses();

	void Reset();

	void ConvertWVRPosesToUnrealPoses(float meterToWorldUnit);

	WVR_DevicePosePair_t wvrPoses[WVR_DEVICE_COUNT_LEVEL_1]; // HMD, R, L controllers.

	int deviceIndexMap[WVR_DEVICE_COUNT_LEVEL_1];  // Index is follow PoseManagerImp::DeviceTypes. Value is the index of wvrPoses.
	FVector DevicePosition[WVR_DEVICE_COUNT_LEVEL_1];  // In Unreal world space.  Index is follow PoseManagerImp::DeviceTypes.
	FQuat DeviceOrientation[WVR_DEVICE_COUNT_LEVEL_1];  // In Unreal world space.  Index is follow PoseManagerImp::DeviceTypes.
};

class FFrameData : public TSharedFromThis<FFrameData, ESPMode::ThreadSafe>
{
public:
	uint32 frameNumber;  // copy from GFrameNumber

	FQuat orientation;  // HMD
	FVector position;  // HMD

	FQuat baseOrientation;  // Game World base
	FVector basePosition;  // Game World base

	FQuat gameOrientation;  // HMD Game World
	FVector gamePosition;  // HMD Game World

	float meterToWorldUnit;  // Game World

	bool bSupportLateUpdate;
	bool bNeedLateUpdateInRT;
	bool bDoUpdateInGT;
	float predictTimeInGT;

	WVR_PoseOriginModel Origin;

	FFramePoses poses;

public:
	FFrameData();

public:
	void ApplyGamePoseForHMD();

	static void DebugLogFrameData(const TSharedPtr<FFrameData, ESPMode::ThreadSafe>& frameData, const char* name);
	static TSharedPtr<FFrameData, ESPMode::ThreadSafe> NewInstance();
	static void Copy(const TSharedPtr<FFrameData, ESPMode::ThreadSafe>& from, TSharedPtr<FFrameData, ESPMode::ThreadSafe>& to);
};

typedef TSharedPtr<FFrameData, ESPMode::ThreadSafe> FrameDataPtr;

