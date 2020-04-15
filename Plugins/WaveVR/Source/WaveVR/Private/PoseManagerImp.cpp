// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "PoseManagerImp.h"
#include "WaveVRUtils.h"
#include "Platforms/WaveVRLogWrapper.h"

DEFINE_LOG_CATEGORY_STATIC(PoseMgrImp, Log, All);

using namespace wvr::utils;

const WVR_DeviceType PoseManagerImp::DeviceTypes[WVR_DEVICE_COUNT_LEVEL_1] = {
	WVR_DeviceType_HMD,
	WVR_DeviceType_Controller_Right,
	WVR_DeviceType_Controller_Left
};

PoseManagerImp* PoseManagerImp::GetInstance()
{
	LOG_FUNC();
	static PoseManagerImp* mInst = new PoseManagerImp();
	return mInst;
}


PoseManagerImp::Device* PoseManagerImp::GetDevice(WVR_DeviceType Type)
{
	LOG_FUNC();
	Device* device = nullptr;
	switch(Type) {
		case WVR_DeviceType_HMD:
			device = hmd;
			break;
		case WVR_DeviceType_Controller_Right:
			device = controllerRight;
			break;
		case WVR_DeviceType_Controller_Left:
			device = controllerLeft;
			break;
	}
	return device;
}

PoseManagerImp::PoseManagerImp()
{
	LOG_FUNC();
	hmd = new Device(WVR_DeviceType_HMD);
	controllerLeft = new Device(WVR_DeviceType_Controller_Left);
	controllerRight = new Device(WVR_DeviceType_Controller_Right);
	CheckSupportedNumOfDoF();
}

PoseManagerImp::~PoseManagerImp()
{
	LOG_FUNC();
	delete hmd;
	delete controllerLeft;
	delete controllerRight;
}

bool PoseManagerImp::IsDeviceConnected(WVR_DeviceType Type) {
	LOG_FUNC();
	return FWaveVRAPIWrapper::GetInstance()->IsDeviceConnected(Type);
}

bool PoseManagerImp::IsDevicePoseValid(WVR_DeviceType Type) {
	LOG_FUNC();
	Device* device = GetDevice(Type);
	return device->pose.pose.isValidPose;
}

void PoseManagerImp::CheckSupportedNumOfDoF() {
	LOG_FUNC();
	if (FWaveVRAPIWrapper::GetInstance()->GetDegreeOfFreedom(WVR_DeviceType::WVR_DeviceType_HMD) == WVR_NumDoF::WVR_NumDoF_6DoF) {
		HmdSupportedDof = EWVR_DOF::DOF_6;
	}
	if (FWaveVRAPIWrapper::GetInstance()->GetDegreeOfFreedom(WVR_DeviceType::WVR_DeviceType_Controller_Right) == WVR_NumDoF::WVR_NumDoF_6DoF) {
		ControllerSupportedDof = EWVR_DOF::DOF_6;
	}
}

EWVR_DOF PoseManagerImp::GetSupportedNumOfDoF(WVR_DeviceType Type) {
	LOG_FUNC();
	switch (Type)
	{
		case WVR_DeviceType::WVR_DeviceType_HMD:
			return HmdSupportedDof;
		case WVR_DeviceType::WVR_DeviceType_Controller_Right:
		case WVR_DeviceType::WVR_DeviceType_Controller_Left:
			return ControllerSupportedDof;
	}
	return EWVR_DOF::DOF_3;
}

EWVR_DOF PoseManagerImp::GetNumOfDoF(WVR_DeviceType Type) {
		LOG_FUNC()
	EWVR_DOF result = GetSupportedNumOfDoF(Type);
	switch (Type)
	{
		case WVR_DeviceType::WVR_DeviceType_HMD:
			if(IsTrackingHMDPosition() == false) {
				return EWVR_DOF::DOF_3;
			}
			return result;
		case WVR_DeviceType::WVR_DeviceType_Controller_Right:
		case WVR_DeviceType::WVR_DeviceType_Controller_Left:
			return result;
	}
	return EWVR_DOF::DOF_3;
}

bool PoseManagerImp::SetNumOfDoF(WVR_DeviceType Type, EWVR_DOF DOF) {
		LOG_FUNC();
	bool enablePoseTracking = true;
	switch (Type)
	{
		case WVR_DeviceType::WVR_DeviceType_HMD:
			if (DOF == EWVR_DOF::DOF_3) {
				enablePoseTracking = false;
			}
			SetTrackingHMDPosition(enablePoseTracking);
			return true;
		case WVR_DeviceType::WVR_DeviceType_Controller_Right:
		case WVR_DeviceType::WVR_DeviceType_Controller_Left:
			return false; //Do not support set Dof of controller now.
	}
	return false;
}

void PoseManagerImp::SetTrackingHMDPosition(bool IsTrackingPosition) {
	LOG_FUNC();
	bIsHMDTrackingPosition = IsTrackingPosition;
}

bool PoseManagerImp::IsTrackingHMDPosition() {
	LOG_FUNC();
	return bIsHMDTrackingPosition;
}

void PoseManagerImp::SetTrackingHMDRotation(bool IsTrackingRotation) {
	LOG_FUNC();
	bIsHMDTrackingRotation = IsTrackingRotation;
}

void PoseManagerImp::SetTrackingOrigin3Dof() {
	LOG_FUNC();
	Origin = WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnHead_3DoF;
}

void PoseManagerImp::SetTrackingOriginPoses(EHMDTrackingOrigin::Type NewOrigin) {
	LOG_FUNC();
	switch (NewOrigin)
		{
		case EHMDTrackingOrigin::Eye:
			Origin = WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnHead;
			break;
		case EHMDTrackingOrigin::Floor:
			Origin = WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnGround;
			break;
		}
}

EHMDTrackingOrigin::Type PoseManagerImp::GetTrackingOriginPoses() {
	LOG_FUNC();
	EHMDTrackingOrigin::Type rv = EHMDTrackingOrigin::Eye;

	switch (Origin)
		{
		case WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnHead:
			rv = EHMDTrackingOrigin::Eye;
			break;
		case WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnGround:
			rv = EHMDTrackingOrigin::Floor;
			break;
		}

	return rv;
}

WVR_PoseOriginModel PoseManagerImp::GetTrackingOriginModelInternal() {
	return Origin;
}

void PoseManagerImp::UpdateDevice(FrameDataPtr frameData) {
	LOG_FUNC();
	Device* devices[3] = {hmd, controllerLeft, controllerRight};
	for (auto dev : devices) {
		check(dev);
		auto& framePoses = frameData->poses;
		auto& posePairs = framePoses.wvrPoses;

		int idxMapResult = framePoses.deviceIndexMap[dev->index];
		if (idxMapResult >= 0 && idxMapResult < WVR_DEVICE_COUNT_LEVEL_1)
		{
			dev->pose = posePairs[idxMapResult];
			dev->orientation = framePoses.DeviceOrientation[dev->index];
			dev->position = framePoses.DevicePosition[dev->index];
			dev->rotation = dev->orientation.Rotator();

			// Teleport
			WaveVRUtils::ApplyGamePose(dev->orientation, dev->position, frameData->baseOrientation, frameData->basePosition, dev->orientation_CompoundBase, dev->position_CompoundBase);

			dev->rotation_CompoundBase = dev->orientation_CompoundBase.Rotator();
		}
		else
		{
			// If didn't have idxMapResult, device's pose may be invalid, and the mapped index will be unpredictable.  Keep other poses variables.
			dev->pose.pose.isValidPose = false;
		}
	}

	// Position/Rotation of HMD checking.
	if (!bIsHMDTrackingPosition) { hmd->position = hmd->position_CompoundBase = FVector::ZeroVector; }
	if (!bIsHMDTrackingRotation) {
		hmd->orientation = hmd->orientation_CompoundBase = FQuat::Identity;
		hmd->rotation_CompoundBase = FRotator::ZeroRotator;
	}

	//frameData->orientation = hmd->orientation;
	//frameData->position = hmd->position;
	frameData->gameOrientation = hmd->orientation_CompoundBase;
	frameData->gamePosition = hmd->position_CompoundBase;
}

bool PoseManagerImp::LateUpdate_RenderThread(FrameDataPtr frameData)
{
	LOG_FUNC();
	SCOPED_NAMED_EVENT(LateUpdatePoses, FColor::Magenta);

	//Update Devices pose from runtime
	auto& framePoses = frameData->poses;
	auto& posePairs = framePoses.wvrPoses;

	WVR()->GetSyncPose(frameData->Origin, posePairs, WVR_DEVICE_COUNT_LEVEL_1);

	framePoses.ConvertWVRPosesToUnrealPoses(frameData->meterToWorldUnit);

	// Should not update Devices of PoseManager in RenderingThread.
	//UpdateDevice(frameData);

	// We only need the HMD.  Do our own.
	frameData->orientation = framePoses.DeviceOrientation[0];  // 0 if for HMD
	frameData->position = framePoses.DevicePosition[0];  // 0 if for HMD
	frameData->ApplyGamePoseForHMD();

	if (!bIsHMDTrackingPosition) { frameData->position = frameData->gamePosition = FVector::ZeroVector; }
	if (!bIsHMDTrackingRotation) { frameData->orientation = frameData->gameOrientation = FQuat::Identity; }

	return posePairs[0].pose.isValidPose;
}

/*Update all poses and return the assigned device position and Orientation*/
void PoseManagerImp::UpdatePoses(FrameDataPtr frameData)
{
	LOG_FUNC();
	SCOPED_NAMED_EVENT(UpdatePoses, FColor::Orange);

	auto& framePoses = frameData->poses;
	auto& posePairs = framePoses.wvrPoses;

	check(IsInGameThread());
	if (frameData->bSupportLateUpdate) {
		if (frameData->bDoUpdateInGT) {
			SCOPED_NAMED_EVENT(GetPoseState, FColor::Blue);
			float predictMS = frameData->predictTimeInGT;
			posePairs[0].type = WVR_DeviceType_HMD;
			WVR()->GetPoseState(WVR_DeviceType_HMD, Origin, predictMS, &(posePairs[0].pose));
			posePairs[1].type = WVR_DeviceType_Controller_Left;
			WVR()->GetPoseState(WVR_DeviceType_Controller_Left, Origin, predictMS, &(posePairs[1].pose));
			posePairs[2].type = WVR_DeviceType_Controller_Right;
			WVR()->GetPoseState(WVR_DeviceType_Controller_Right, Origin, predictMS, &(posePairs[2].pose));
		}
	} else {
		SCOPED_NAMED_EVENT(GetSyncPose, FColor::Orange);
		FRenderCommandFence Fence_Pose; //Avoid GetSyncPose twice in one submit.
		Fence_Pose.BeginFence();
		Fence_Pose.Wait();
		WVR()->GetSyncPose(frameData->Origin, posePairs, WVR_DEVICE_COUNT_LEVEL_1);
	}

	framePoses.ConvertWVRPosesToUnrealPoses(frameData->meterToWorldUnit);
	UpdateDevice(frameData); //HMD , L-controller, R-controller

#if DEBUG
	PrintDeviceInfo(frameData);
#endif
}

void PoseManagerImp::PrintDeviceInfo(FrameDataPtr frameData) {
	Device* devices[3] = {hmd, controllerLeft, controllerRight};
	for(auto dev : devices) {
		if (dev == hmd) {
			LOGD(PoseMgrImp, "HMD Info: ");
		} else if(dev == controllerLeft){
			LOGD(PoseMgrImp, "Left Controller Info: ");
		} else if(dev == controllerRight){
			LOGD(PoseMgrImp, "Right Controller Info: ");
		}
		if(dev->pose.pose.isValidPose){
			LOGD(PoseMgrImp, "Device is valid");
		}else{
			LOGD(PoseMgrImp, "Device is NOT valid");
		}

		FQuat BaseOrientation = frameData->baseOrientation;
		FVector CompensationOffset = frameData->basePosition;
		LOGD(PoseMgrImp, "BaseOrientation(Pitch, Roll, Yaw) is (%f, %f, %f), CompensationOffset(X,Y,Z) is (%f, %f, %f)",
				BaseOrientation.Rotator().Pitch, BaseOrientation.Rotator().Roll, BaseOrientation.Rotator().Yaw,
				CompensationOffset.X, CompensationOffset.Y, CompensationOffset.Z );
		LOGD(PoseMgrImp, "Position_CompoundBase(X,Y,Z) is (%f, %f, %f)", dev->position_CompoundBase.X, dev->position_CompoundBase.Y, dev->position_CompoundBase.Z);
		LOGD(PoseMgrImp, "Orientation_CompoundBase (W,X,Y,Z) is (%f, %f, %f, %f)",
			dev->orientation_CompoundBase.W, dev->orientation_CompoundBase.X, dev->orientation_CompoundBase.Y, dev->orientation_CompoundBase.Z);
		LOGD(PoseMgrImp, "Rotator_CompoundBase(Pitch,Roll,Yaw) is(%f, %f, %f)", dev->rotation_CompoundBase.Pitch, dev->rotation_CompoundBase.Roll, dev->rotation_CompoundBase.Yaw);
	}
}
