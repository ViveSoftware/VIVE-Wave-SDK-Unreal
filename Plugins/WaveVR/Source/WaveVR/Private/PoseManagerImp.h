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
#include "WaveVRBlueprintFunctionLibrary.h"
#include "WaveVRHMD_FrameData.h"
#include "HeadMountedDisplayTypes.h"

class PoseManagerImp
{
public:
	static const WVR_DeviceType DeviceTypes[WVR_DEVICE_COUNT_LEVEL_1];
	/*poses derived from WVR_GetSyncPose directly*/

	class Device
	{
	public:
		Device(WVR_DeviceType _type)
		{
			type = _type;
			for (int i = 0; i < WVR_DEVICE_COUNT_LEVEL_1; i++)
			{
				if (DeviceTypes[i] == type)
				{
					index = i;
					break;
				}
			}
			pose.pose.isValidPose = false;
		}

		WVR_DeviceType type;
		int index;
		WVR_DevicePosePair_t pose;
		FVector position;
		FQuat orientation;
		FRotator rotation;
		FVector position_CompoundBase;
		FQuat orientation_CompoundBase;
		FRotator rotation_CompoundBase;
	};
public:
	static PoseManagerImp* GetInstance();
	virtual ~PoseManagerImp();
	void UpdatePoses(FrameDataPtr frameData); //Keep update per game frame
	bool LateUpdate_RenderThread(FrameDataPtr frameData);

public:
	Device* GetDevice(WVR_DeviceType Type);
	bool IsDeviceConnected(WVR_DeviceType Type);
	bool IsDevicePoseValid(WVR_DeviceType Type);
	void CheckSupportedNumOfDoF();
	EWVR_DOF GetSupportedNumOfDoF(WVR_DeviceType Type);
	EWVR_DOF GetNumOfDoF(WVR_DeviceType Type);
	bool SetNumOfDoF(WVR_DeviceType Type, EWVR_DOF Dof);
	void SetTrackingHMDPosition(bool IsTrackingPosition);
	bool IsTrackingHMDPosition();
	void SetTrackingHMDRotation(bool IsTrackingRotation);
	void SetTrackingOrigin3Dof();
	void SetTrackingOriginPoses(EHMDTrackingOrigin::Type NewOrigin);

	EHMDTrackingOrigin::Type GetTrackingOriginPoses();
	WVR_PoseOriginModel GetTrackingOriginModelInternal();

public:
	void UpdateDevice(FrameDataPtr frameData);

private:
	PoseManagerImp(); //prevent new instance directly

	void PrintDeviceInfo(FrameDataPtr frameData);

private:
	Device* hmd = nullptr;
	Device* controllerLeft = nullptr;
	Device* controllerRight = nullptr;
	WVR_PoseOriginModel Origin = WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnGround;
	EWVR_DOF HmdSupportedDof = EWVR_DOF::DOF_3;
	EWVR_DOF ControllerSupportedDof = EWVR_DOF::DOF_3;
	bool bIsHMDTrackingPosition = true;
	bool bIsHMDTrackingRotation = true;
};
