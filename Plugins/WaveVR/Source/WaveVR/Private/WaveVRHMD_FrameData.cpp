// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRHMD_FrameData.h"
#include "WaveVRUtils.h"
#include "PoseManagerImp.h"
#include "Platforms/WaveVRLogWrapper.h"

DEFINE_LOG_CATEGORY_STATIC(WVRFrameData, Display, All);

using namespace wvr::utils;

FFramePoses::FFramePoses()
{
	Reset();
}

void FFramePoses::Reset()
{
	for (int i = 0; i < WVR_DEVICE_COUNT_LEVEL_1; i++)
	{
		wvrPoses->pose.isValidPose = false;
		deviceIndexMap[i] = -1;
		DeviceOrientation[i] = FQuat::Identity;
		DevicePosition[i] = FVector::ZeroVector;
	}
}

void FFramePoses::ConvertWVRPosesToUnrealPoses(float meterToWorldUnit) {
	// Because the index of wvrPoses may not follow type order.
	// Sort index in the type order of HMD, Left, Right.
	for (uint32_t i = 0; i < /*DeviceTypes.Length*/WVR_DEVICE_COUNT_LEVEL_1; i++)
	{
		bool hasType = false;
		deviceIndexMap[i] = -1;

		for (uint32_t j = 0; j < /*poses.Length*/WVR_DEVICE_COUNT_LEVEL_1; j++)
		{
			const WVR_DevicePosePair_t& wvrPose = wvrPoses[j];

			if (wvrPose.type == PoseManagerImp::DeviceTypes[i])
			{
				hasType = true;
				deviceIndexMap[i] = j;

				if (wvrPose.pose.isValidPose)
					WaveVRUtils::ConvertWVRMatrixToUnrealPose(wvrPose.pose.poseMatrix, meterToWorldUnit, DeviceOrientation[i], DevicePosition[i]);
				break;
			}
			else if (wvrPose.type == 100) { //[WVR] has type id 100 if Device disconnected.
				hasType = true;
				break;
			}
		}
//#if DEBUG
		if (!hasType) LOGD(WVRFrameData, "This pose belongs to no supported type");
//#endif
	}
}


FFrameData::FFrameData()
	: frameNumber(0)
	, orientation(EForceInit::ForceInit)
	, position()
	, baseOrientation(EForceInit::ForceInit)
	, basePosition()
	, gameOrientation(EForceInit::ForceInit)
	, gamePosition()
	, meterToWorldUnit(100)
	, bSupportLateUpdate(true)
	, bNeedLateUpdateInRT(true)
	, bDoUpdateInGT(false)
	, predictTimeInGT(0.0f)
	, Origin(WVR_PoseOriginModel_OriginOnHead)
	, poses()
{
}

void FFrameData::ApplyGamePoseForHMD() {
	WaveVRUtils::ApplyGamePose(orientation, position, baseOrientation, basePosition, gameOrientation, gamePosition);
}

FrameDataPtr FFrameData::NewInstance() {
	return MakeShareable(new FFrameData());
}

void FFrameData::DebugLogFrameData(const TSharedPtr<FFrameData, ESPMode::ThreadSafe>& frameData, const char* name) {
	LOGD(WVRFrameData,
		"FrameData: %s\n"
		"frameNumber=%d, bNeedLateUpdateInRT=%d, meterToWorldUnit=%f, Origin=%d\n"
		"position=(%6f, %6f, %6f) orientation=(%.4f, %.4f, %.4f, %.4f)\n"
		"basePosition=(%6f, %6f, %6f) baseOrientation=(%.4f, %.4f, %.4f, %.4f)\n"
		"gamePosition=(%6f, %6f, %6f) gameOrientation=(%.4f, %.4f, %.4f, %.4f)\n",
		USEWIDE(name),
		frameData->frameNumber, frameData->bNeedLateUpdateInRT, frameData->meterToWorldUnit, frameData->Origin,
		frameData->position.X, frameData->position.Y, frameData->position.Z,
		frameData->orientation.X, frameData->orientation.Y, frameData->orientation.Z, frameData->orientation.W,
		frameData->basePosition.X, frameData->basePosition.Y, frameData->basePosition.Z,
		frameData->baseOrientation.X, frameData->baseOrientation.Y, frameData->baseOrientation.Z, frameData->baseOrientation.W,
		frameData->gamePosition.X, frameData->gamePosition.Y, frameData->gamePosition.Z,
		frameData->gameOrientation.X, frameData->gameOrientation.Y, frameData->gameOrientation.Z, frameData->gameOrientation.W
	);
}

void FFrameData::Copy(const TSharedPtr<FFrameData, ESPMode::ThreadSafe>& from, TSharedPtr<FFrameData, ESPMode::ThreadSafe>& to) {
	*to = *from;
}
