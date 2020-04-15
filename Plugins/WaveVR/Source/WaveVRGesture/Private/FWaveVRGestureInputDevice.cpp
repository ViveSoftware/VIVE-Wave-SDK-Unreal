// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "IWaveVRGesture.h"
#include "FWaveVRGestureInputDevice.h"
#include "WaveVRStaticGestureComponent.h"
#include "WaveVRUtils.h"

using namespace wvr::utils;

FWaveVRGestureInputDevice::FWaveVRGestureInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) :
	MessageHandler(InMessageHandler),
	DeviceIndex(0),
	bHandGestureDataUpdated(false),
	prevStaticGestureRight(EWaveVRStaticGesture::TYPE_INVALID),
	prevStaticGestureLeft(EWaveVRStaticGesture::TYPE_INVALID),
	currStaticGestureRight(EWaveVRStaticGesture::TYPE_INVALID),
	currStaticGestureLeft(EWaveVRStaticGesture::TYPE_INVALID),
	bHandTrackingDataUpdated(false),
	World(nullptr),
	BONE_OFFSET_L(FVector(0, 0, 0)),
	BONE_OFFSET_R(FVector(0, 0, 0))
{
	Skeleton = new WaveVRGestureSkeleton();
	bGestureInputDeviceInitialized = true;
	gestureThread = FWaveVRGestureThread::JoyInit();
	UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("FWaveVRGestureInputDevice()"));
}


FWaveVRGestureInputDevice::~FWaveVRGestureInputDevice()
{
}


void FWaveVRGestureInputDevice::Tick(float DeltaTime)
{
	FWaveVRHMD* HMD = GetWaveVRHMD();

	if (!bGestureInputDeviceInitialized
		|| (gestureThread == NULL)
		|| (HMD == nullptr))
	{
		bHandGestureDataUpdated = false;
		bHandTrackingDataUpdated = false;
		return;
	}

	/* ----------------------- Hand Gesture -----------------------*/
	WVR_EventType gesture_event = HMD->GetGestureEvent();

	if (gesture_event == WVR_EventType::WVR_EventType_HandGesture_Abnormal)
	{
		gestureThread->RestartHandGesture();
		HMD->SetGestureEvent(WVR_EventType::WVR_EventType_HandGesture_Changed);
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("Tick() hand gesture is abnormal, restart the feature."));
	}

	if (gestureThread->IsHandGestureAvailable())
	{
		bHandGestureDataUpdated = FWaveVRAPIWrapper::GetInstance()->GetHandGestureData(&handGestureData) == WVR_Result::WVR_Success ? true : false;
		if (bHandGestureDataUpdated)
		{
			UpdateLeftHandGestureData(handGestureData);
			UpdateRightHandGestureData(handGestureData);
		}
	}



	/* ----------------------- Hand Tracking -----------------------*/
	if (gesture_event == WVR_EventType::WVR_EventType_HandTracking_Abnormal)
	{
		gestureThread->RestartHandTracking();
		HMD->SetGestureEvent(WVR_EventType::WVR_EventType_HandGesture_Changed);
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("Tick() hand tracking is abnormal, restart the feature."));
	}

	if (gestureThread->IsHandTrackingAvailable())
	{
		UpdateParametersFromPawn();

		bHandTrackingDataUpdated = FWaveVRAPIWrapper::GetInstance()->GetHandTrackingData(&handTrackingData) == WVR_Result::WVR_Success ? true : false;
		if (bHandTrackingDataUpdated)	// Get the tracking data from the shmem.
		{
			UpdateLeftHandTrackingData(handTrackingData);
			UpdateRightHandTrackingData(handTrackingData);
		}
	}
}


void FWaveVRGestureInputDevice::SendControllerEvents()
{
}


void FWaveVRGestureInputDevice::SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}


bool FWaveVRGestureInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	// Nothing necessary to do (boilerplate code to complete the interface)
	return false;
}


void FWaveVRGestureInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	// Nothing necessary to do (boilerplate code to complete the interface)
}


void FWaveVRGestureInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{
	// Nothing necessary to do (boilerplate code to complete the interface)
}

#pragma region
bool FWaveVRGestureInputDevice::IsGestureInputDeviceInitialized()
{
	return bGestureInputDeviceInitialized;
}

void FWaveVRGestureInputDevice::StartHandGesture()
{
	if (gestureThread != NULL)
		gestureThread->StartHandGesture();
}

void FWaveVRGestureInputDevice::StopHandGesture()
{
	if (gestureThread != NULL)
		gestureThread->StopHandGesture();
}

bool FWaveVRGestureInputDevice::IsHandGestureAvailable()
{
	if (gestureThread != NULL)
		return gestureThread->IsHandGestureAvailable();

	return false;
}

EWaveVRStaticGesture FWaveVRGestureInputDevice::GetStaticGestureType(EWaveVRGestureHandType DevType)
{
	if (!bHandGestureDataUpdated)
		return EWaveVRStaticGesture::TYPE_INVALID;

	if (DevType == EWaveVRGestureHandType::LEFT)
		return currStaticGestureLeft;
	if (DevType == EWaveVRGestureHandType::RIGHT)
		return currStaticGestureRight;

	return EWaveVRStaticGesture::TYPE_INVALID;
}

EWaveVRHandGestureStatus FWaveVRGestureInputDevice::GetHandGestureStatus()
{
	if (gestureThread != NULL)
		return gestureThread->GetHandGestureStatus();

	return EWaveVRHandGestureStatus::UNSUPPORT;
}

void FWaveVRGestureInputDevice::StartHandTracking()
{
	if (gestureThread != NULL)
		gestureThread->StartHandTracking();
}

void FWaveVRGestureInputDevice::StopHandTracking()
{
	if (gestureThread != NULL)
		gestureThread->StopHandTracking();
}

bool FWaveVRGestureInputDevice::IsHandTrackingAvailable()
{
	if (gestureThread != NULL)
		return gestureThread->IsHandTrackingAvailable();

	return false;
}

bool FWaveVRGestureInputDevice::GetBonePositionAndRotation(int32 DevId, EWaveVRGestureBoneType BoneId, FVector& OutPosition, FRotator& OutRotation)
{
	if (!bHandTrackingDataUpdated
		|| DevId < 0
		|| ((int32)BoneId >= (int32)EWaveVRGestureBoneType::BONES_COUNT)
		|| ((int32)BoneId <= (int32)EWaveVRGestureBoneType::BONE_ROOT)
		)
		return false;

	OutPosition = Skeleton->Bones[(int32)BoneId]->GetPosition();
	OutRotation = Skeleton->Bones[(int32)BoneId]->GetRotation();
	return Skeleton->Bones[(int32)BoneId]->IsValidPose();
}

EWaveVRHandTrackingStatus FWaveVRGestureInputDevice::GetHandTrackingStatus()
{
	if (gestureThread != NULL)
		return gestureThread->GetHandTrackingStatus();

	return EWaveVRHandTrackingStatus::UNSUPPORT;
}
#pragma endregion GestureBPLibrary related functions.

#pragma region
void FWaveVRGestureInputDevice::UpdateLeftHandTrackingData(WVR_HandTrackingData tracking_data)
{
	if (Skeleton == nullptr) return;

	FWaveVRHMD* HMD = GetWaveVRHMD();
	if (HMD == nullptr) return;

	// Left arm wrist - BONE_HAND_WRIST_L.
	FVector BONE_HAND_WRIST_L_Pos = FVector::ZeroVector;
	FQuat BONE_HAND_WRIST_L_Quat = FQuat::Identity;
	CoordinateUtil::MatrixToPose(tracking_data.left.poseMatrix, BONE_HAND_WRIST_L_Quat, BONE_HAND_WRIST_L_Pos, HMD->GetWorldToMetersScale());

	/*BONE_HAND_WRIST_L_Pos.X = tracking_data.left.rawPose.position.v[0];
	BONE_HAND_WRIST_L_Pos.Y = tracking_data.left.rawPose.position.v[1];
	BONE_HAND_WRIST_L_Pos.Z = tracking_data.left.rawPose.position.v[2];
	BONE_HAND_WRIST_L_Pos = CoordinateUtil::GetVector3(BONE_HAND_WRIST_L_Pos, HMD->GetWorldToMetersScale());
	BONE_HAND_WRIST_L_Quat.W = tracking_data.left.rawPose.rotation.w;
	BONE_HAND_WRIST_L_Quat.X = tracking_data.left.rawPose.rotation.x;
	BONE_HAND_WRIST_L_Quat.Y = tracking_data.left.rawPose.rotation.y;
	BONE_HAND_WRIST_L_Quat.Z = tracking_data.left.rawPose.rotation.z;
	BONE_HAND_WRIST_L_Quat = CoordinateUtil::GetQuaternion(BONE_HAND_WRIST_L_Quat);*/

	BONE_HAND_WRIST_L_Pos += BONE_OFFSET_L;
	FRotator BONE_HAND_WRIST_L_Rot = BONE_HAND_WRIST_L_Quat.Rotator();

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_L]->SetPosition(BONE_HAND_WRIST_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_L]->SetRotation(BONE_HAND_WRIST_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm thumb joint1 bone - BONE_THUMB_JOINT1_L.
	FVector BONE_THUMB_JOINT1_L_Pos = FVector::ZeroVector;
	BONE_THUMB_JOINT1_L_Pos.X = tracking_data.leftFingers.thumb.joint1.v[0];
	BONE_THUMB_JOINT1_L_Pos.Y = tracking_data.leftFingers.thumb.joint1.v[1];
	BONE_THUMB_JOINT1_L_Pos.Z = tracking_data.leftFingers.thumb.joint1.v[2];
	BONE_THUMB_JOINT1_L_Pos = CoordinateUtil::GetVector3(BONE_THUMB_JOINT1_L_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_THUMB_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_L]->SetPosition(BONE_THUMB_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_L]->SetRotation(BONE_THUMB_JOINT1_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm thumb joint2 bone - BONE_THUMB_JOINT2_L.
	FVector BONE_THUMB_JOINT2_L_Pos = FVector::ZeroVector;
	BONE_THUMB_JOINT2_L_Pos.X = tracking_data.leftFingers.thumb.joint2.v[0];
	BONE_THUMB_JOINT2_L_Pos.Y = tracking_data.leftFingers.thumb.joint2.v[1];
	BONE_THUMB_JOINT2_L_Pos.Z = tracking_data.leftFingers.thumb.joint2.v[2];
	BONE_THUMB_JOINT2_L_Pos = CoordinateUtil::GetVector3(BONE_THUMB_JOINT2_L_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_THUMB_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_L]->SetPosition(BONE_THUMB_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_L]->SetRotation(BONE_THUMB_JOINT2_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm thumb joint3 bone - BONE_THUMB_JOINT3_L.
	FVector BONE_THUMB_JOINT3_L_Pos = FVector::ZeroVector;
	BONE_THUMB_JOINT3_L_Pos.X = tracking_data.leftFingers.thumb.joint3.v[0];
	BONE_THUMB_JOINT3_L_Pos.Y = tracking_data.leftFingers.thumb.joint3.v[1];
	BONE_THUMB_JOINT3_L_Pos.Z = tracking_data.leftFingers.thumb.joint3.v[2];
	BONE_THUMB_JOINT3_L_Pos = CoordinateUtil::GetVector3(BONE_THUMB_JOINT3_L_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT2_L_Pos, BONE_THUMB_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_L]->SetPosition(BONE_THUMB_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_L]->SetRotation(BONE_THUMB_JOINT3_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm thumb tip bone - BONE_THUMB_TIP_L.
	FVector BONE_THUMB_TIP_L_Pos = FVector::ZeroVector;
	BONE_THUMB_TIP_L_Pos.X = tracking_data.leftFingers.thumb.tip.v[0];
	BONE_THUMB_TIP_L_Pos.Y = tracking_data.leftFingers.thumb.tip.v[1];
	BONE_THUMB_TIP_L_Pos.Z = tracking_data.leftFingers.thumb.tip.v[2];
	BONE_THUMB_TIP_L_Pos = CoordinateUtil::GetVector3(BONE_THUMB_TIP_L_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_THUMB_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT3_L_Pos, BONE_THUMB_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_L]->SetPosition(BONE_THUMB_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_L]->SetRotation(BONE_THUMB_TIP_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm index joint1 bone - BONE_INDEX_JOINT1_L.
	FVector BONE_INDEX_JOINT1_L_Pos = FVector::ZeroVector;
	BONE_INDEX_JOINT1_L_Pos.X = tracking_data.leftFingers.index.joint1.v[0];
	BONE_INDEX_JOINT1_L_Pos.Y = tracking_data.leftFingers.index.joint1.v[1];
	BONE_INDEX_JOINT1_L_Pos.Z = tracking_data.leftFingers.index.joint1.v[2];
	BONE_INDEX_JOINT1_L_Pos = CoordinateUtil::GetVector3(BONE_INDEX_JOINT1_L_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_INDEX_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_L]->SetPosition(BONE_INDEX_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_L]->SetRotation(BONE_INDEX_JOINT1_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm index joint2 bone - BONE_INDEX_JOINT2_L.
	FVector BONE_INDEX_JOINT2_L_Pos = FVector::ZeroVector;
	BONE_INDEX_JOINT2_L_Pos.X = tracking_data.leftFingers.index.joint2.v[0];
	BONE_INDEX_JOINT2_L_Pos.Y = tracking_data.leftFingers.index.joint2.v[1];
	BONE_INDEX_JOINT2_L_Pos.Z = tracking_data.leftFingers.index.joint2.v[2];
	BONE_INDEX_JOINT2_L_Pos = CoordinateUtil::GetVector3(BONE_INDEX_JOINT2_L_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT1_L_Pos, BONE_INDEX_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_L]->SetPosition(BONE_INDEX_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_L]->SetRotation(BONE_INDEX_JOINT2_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm index joint3 bone - BONE_INDEX_JOINT3_L.
	FVector BONE_INDEX_JOINT3_L_Pos = FVector::ZeroVector;
	BONE_INDEX_JOINT3_L_Pos.X = tracking_data.leftFingers.index.joint3.v[0];
	BONE_INDEX_JOINT3_L_Pos.Y = tracking_data.leftFingers.index.joint3.v[1];
	BONE_INDEX_JOINT3_L_Pos.Z = tracking_data.leftFingers.index.joint3.v[2];
	BONE_INDEX_JOINT3_L_Pos = CoordinateUtil::GetVector3(BONE_INDEX_JOINT3_L_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT2_L_Pos, BONE_INDEX_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_L]->SetPosition(BONE_INDEX_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_L]->SetRotation(BONE_INDEX_JOINT3_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm index tip bone - BONE_INDEX_TIP_L.
	FVector BONE_INDEX_TIP_L_Pos = FVector::ZeroVector;
	BONE_INDEX_TIP_L_Pos.X = tracking_data.leftFingers.index.tip.v[0];
	BONE_INDEX_TIP_L_Pos.Y = tracking_data.leftFingers.index.tip.v[1];
	BONE_INDEX_TIP_L_Pos.Z = tracking_data.leftFingers.index.tip.v[2];
	BONE_INDEX_TIP_L_Pos = CoordinateUtil::GetVector3(BONE_INDEX_TIP_L_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_INDEX_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT3_L_Pos, BONE_INDEX_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_L]->SetPosition(BONE_INDEX_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_L]->SetRotation(BONE_INDEX_TIP_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm middle joint1 bone - BONE_MIDDLE_JOINT1_L.
	FVector BONE_MIDDLE_JOINT1_L_Pos = FVector::ZeroVector;
	BONE_MIDDLE_JOINT1_L_Pos.X = tracking_data.leftFingers.middle.joint1.v[0];
	BONE_MIDDLE_JOINT1_L_Pos.Y = tracking_data.leftFingers.middle.joint1.v[1];
	BONE_MIDDLE_JOINT1_L_Pos.Z = tracking_data.leftFingers.middle.joint1.v[2];
	BONE_MIDDLE_JOINT1_L_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_JOINT1_L_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_MIDDLE_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_L]->SetPosition(BONE_MIDDLE_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_L]->SetRotation(BONE_MIDDLE_JOINT1_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm middle joint2 bone - BONE_MIDDLE_JOINT2_L.
	FVector BONE_MIDDLE_JOINT2_L_Pos = FVector::ZeroVector;
	BONE_MIDDLE_JOINT2_L_Pos.X = tracking_data.leftFingers.middle.joint2.v[0];
	BONE_MIDDLE_JOINT2_L_Pos.Y = tracking_data.leftFingers.middle.joint2.v[1];
	BONE_MIDDLE_JOINT2_L_Pos.Z = tracking_data.leftFingers.middle.joint2.v[2];
	BONE_MIDDLE_JOINT2_L_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_JOINT2_L_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT1_L_Pos, BONE_MIDDLE_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_L]->SetPosition(BONE_MIDDLE_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_L]->SetRotation(BONE_MIDDLE_JOINT2_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm middle joint3 bone - BONE_MIDDLE_JOINT3_L.
	FVector BONE_MIDDLE_JOINT3_L_Pos = FVector::ZeroVector;
	BONE_MIDDLE_JOINT3_L_Pos.X = tracking_data.leftFingers.middle.joint3.v[0];
	BONE_MIDDLE_JOINT3_L_Pos.Y = tracking_data.leftFingers.middle.joint3.v[1];
	BONE_MIDDLE_JOINT3_L_Pos.Z = tracking_data.leftFingers.middle.joint3.v[2];
	BONE_MIDDLE_JOINT3_L_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_JOINT3_L_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT2_L_Pos, BONE_MIDDLE_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_L]->SetPosition(BONE_MIDDLE_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_L]->SetRotation(BONE_MIDDLE_JOINT3_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm middle tip bone - BONE_MIDDLE_TIP_L.
	FVector BONE_MIDDLE_TIP_L_Pos = FVector::ZeroVector;
	BONE_MIDDLE_TIP_L_Pos.X = tracking_data.leftFingers.middle.tip.v[0];
	BONE_MIDDLE_TIP_L_Pos.Y = tracking_data.leftFingers.middle.tip.v[1];
	BONE_MIDDLE_TIP_L_Pos.Z = tracking_data.leftFingers.middle.tip.v[2];
	BONE_MIDDLE_TIP_L_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_TIP_L_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_MIDDLE_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT3_L_Pos, BONE_MIDDLE_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_L]->SetPosition(BONE_MIDDLE_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_L]->SetRotation(BONE_MIDDLE_TIP_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm ring joint1 bone - BONE_RING_JOINT1_L.
	FVector BONE_RING_JOINT1_L_Pos = FVector::ZeroVector;
	BONE_RING_JOINT1_L_Pos.X = tracking_data.leftFingers.ring.joint1.v[0];
	BONE_RING_JOINT1_L_Pos.Y = tracking_data.leftFingers.ring.joint1.v[1];
	BONE_RING_JOINT1_L_Pos.Z = tracking_data.leftFingers.ring.joint1.v[2];
	BONE_RING_JOINT1_L_Pos = CoordinateUtil::GetVector3(BONE_RING_JOINT1_L_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_RING_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_L]->SetPosition(BONE_RING_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_L]->SetRotation(BONE_RING_JOINT1_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm ring joint2 bone - BONE_RING_JOINT2_L.
	FVector BONE_RING_JOINT2_L_Pos = FVector::ZeroVector;
	BONE_RING_JOINT2_L_Pos.X = tracking_data.leftFingers.ring.joint2.v[0];
	BONE_RING_JOINT2_L_Pos.Y = tracking_data.leftFingers.ring.joint2.v[1];
	BONE_RING_JOINT2_L_Pos.Z = tracking_data.leftFingers.ring.joint2.v[2];
	BONE_RING_JOINT2_L_Pos = CoordinateUtil::GetVector3(BONE_RING_JOINT2_L_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT1_L_Pos, BONE_RING_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_L]->SetPosition(BONE_RING_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_L]->SetRotation(BONE_RING_JOINT2_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm ring joint3 bone - BONE_RING_JOINT3_L.
	FVector BONE_RING_JOINT3_L_Pos = FVector::ZeroVector;
	BONE_RING_JOINT3_L_Pos.X = tracking_data.leftFingers.ring.joint3.v[0];
	BONE_RING_JOINT3_L_Pos.Y = tracking_data.leftFingers.ring.joint3.v[1];
	BONE_RING_JOINT3_L_Pos.Z = tracking_data.leftFingers.ring.joint3.v[2];
	BONE_RING_JOINT3_L_Pos = CoordinateUtil::GetVector3(BONE_RING_JOINT3_L_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT2_L_Pos, BONE_RING_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_L]->SetPosition(BONE_RING_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_L]->SetRotation(BONE_RING_JOINT3_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm ring tip bone - BONE_RING_TIP_L.
	FVector BONE_RING_TIP_L_Pos = FVector::ZeroVector;
	BONE_RING_TIP_L_Pos.X = tracking_data.leftFingers.ring.tip.v[0];
	BONE_RING_TIP_L_Pos.Y = tracking_data.leftFingers.ring.tip.v[1];
	BONE_RING_TIP_L_Pos.Z = tracking_data.leftFingers.ring.tip.v[2];
	BONE_RING_TIP_L_Pos = CoordinateUtil::GetVector3(BONE_RING_TIP_L_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_RING_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT3_L_Pos, BONE_RING_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_L]->SetPosition(BONE_RING_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_L]->SetRotation(BONE_RING_TIP_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm pinky joint1 bone - BONE_PINKY_JOINT1_L.
	FVector BONE_PINKY_JOINT1_L_Pos = FVector::ZeroVector;
	BONE_PINKY_JOINT1_L_Pos.X = tracking_data.leftFingers.pinky.joint1.v[0];
	BONE_PINKY_JOINT1_L_Pos.Y = tracking_data.leftFingers.pinky.joint1.v[1];
	BONE_PINKY_JOINT1_L_Pos.Z = tracking_data.leftFingers.pinky.joint1.v[2];
	BONE_PINKY_JOINT1_L_Pos = CoordinateUtil::GetVector3(BONE_PINKY_JOINT1_L_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT1_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_JOINT1_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_L_Pos, BONE_PINKY_JOINT1_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_L]->SetPosition(BONE_PINKY_JOINT1_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_L]->SetRotation(BONE_PINKY_JOINT1_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm pinky joint2 bone - BONE_PINKY_JOINT2_L.
	FVector BONE_PINKY_JOINT2_L_Pos = FVector::ZeroVector;
	BONE_PINKY_JOINT2_L_Pos.X = tracking_data.leftFingers.pinky.joint2.v[0];
	BONE_PINKY_JOINT2_L_Pos.Y = tracking_data.leftFingers.pinky.joint2.v[1];
	BONE_PINKY_JOINT2_L_Pos.Z = tracking_data.leftFingers.pinky.joint2.v[2];
	BONE_PINKY_JOINT2_L_Pos = CoordinateUtil::GetVector3(BONE_PINKY_JOINT2_L_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT2_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_JOINT2_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT1_L_Pos, BONE_PINKY_JOINT2_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_L]->SetPosition(BONE_PINKY_JOINT2_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_L]->SetRotation(BONE_PINKY_JOINT2_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm pinky joint3 bone - BONE_PINKY_JOINT3_L.
	FVector BONE_PINKY_JOINT3_L_Pos = FVector::ZeroVector;
	BONE_PINKY_JOINT3_L_Pos.X = tracking_data.leftFingers.pinky.joint3.v[0];
	BONE_PINKY_JOINT3_L_Pos.Y = tracking_data.leftFingers.pinky.joint3.v[1];
	BONE_PINKY_JOINT3_L_Pos.Z = tracking_data.leftFingers.pinky.joint3.v[2];
	BONE_PINKY_JOINT3_L_Pos = CoordinateUtil::GetVector3(BONE_PINKY_JOINT3_L_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT3_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_JOINT3_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT2_L_Pos, BONE_PINKY_JOINT3_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_L]->SetPosition(BONE_PINKY_JOINT3_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_L]->SetRotation(BONE_PINKY_JOINT3_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_L]->SetPoseValid(tracking_data.left.isValidPose);

	// Left arm pinky tip bone - BONE_PINKY_TIP_L.
	FVector BONE_PINKY_TIP_L_Pos = FVector::ZeroVector;
	BONE_PINKY_TIP_L_Pos.X = tracking_data.leftFingers.pinky.tip.v[0];
	BONE_PINKY_TIP_L_Pos.Y = tracking_data.leftFingers.pinky.tip.v[1];
	BONE_PINKY_TIP_L_Pos.Z = tracking_data.leftFingers.pinky.tip.v[2];
	BONE_PINKY_TIP_L_Pos = CoordinateUtil::GetVector3(BONE_PINKY_TIP_L_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_TIP_L_Pos += BONE_OFFSET_L;
	FRotator BONE_PINKY_TIP_L_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT3_L_Pos, BONE_PINKY_TIP_L_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_L]->SetPosition(BONE_PINKY_TIP_L_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_L]->SetRotation(BONE_PINKY_TIP_L_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_L]->SetPoseValid(tracking_data.left.isValidPose);

}

void FWaveVRGestureInputDevice::UpdateRightHandTrackingData(WVR_HandTrackingData tracking_data)
{
	if (Skeleton == nullptr) return;

	FWaveVRHMD* HMD = GetWaveVRHMD();
	if (HMD == nullptr) return;

	// Right arm wrist - BONE_HAND_WRIST_R.
	FVector BONE_HAND_WRIST_R_Pos = FVector::ZeroVector;
	FQuat BONE_HAND_WRIST_R_Quat = FQuat::Identity;
	CoordinateUtil::MatrixToPose(tracking_data.right.poseMatrix, BONE_HAND_WRIST_R_Quat, BONE_HAND_WRIST_R_Pos, HMD->GetWorldToMetersScale());

	/*BONE_HAND_WRIST_R_Pos.X = tracking_data.right.rawPose.position.v[0];
	BONE_HAND_WRIST_R_Pos.Y = tracking_data.right.rawPose.position.v[1];
	BONE_HAND_WRIST_R_Pos.Z = tracking_data.right.rawPose.position.v[2];
	BONE_HAND_WRIST_R_Pos = CoordinateUtil::GetVector3(BONE_HAND_WRIST_R_Pos, HMD->GetWorldToMetersScale());
	BONE_HAND_WRIST_R_Quat.W = tracking_data.right.rawPose.rotation.w;
	BONE_HAND_WRIST_R_Quat.X = tracking_data.right.rawPose.rotation.x;
	BONE_HAND_WRIST_R_Quat.Y = tracking_data.right.rawPose.rotation.y;
	BONE_HAND_WRIST_R_Quat.Z = tracking_data.right.rawPose.rotation.z;
	BONE_HAND_WRIST_R_Quat = CoordinateUtil::GetQuaternion(BONE_HAND_WRIST_R_Quat);*/

	BONE_HAND_WRIST_R_Pos += BONE_OFFSET_R;
	FRotator BONE_HAND_WRIST_R_Rot = BONE_HAND_WRIST_R_Quat.Rotator();

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_R]->SetPosition(BONE_HAND_WRIST_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_R]->SetRotation(BONE_HAND_WRIST_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_HAND_WRIST_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm thumb joint1 bone - BONE_THUMB_JOINT1_R.
	FVector BONE_THUMB_JOINT1_R_Pos = FVector::ZeroVector;
	BONE_THUMB_JOINT1_R_Pos.X = tracking_data.rightFingers.thumb.joint1.v[0];
	BONE_THUMB_JOINT1_R_Pos.Y = tracking_data.rightFingers.thumb.joint1.v[1];
	BONE_THUMB_JOINT1_R_Pos.Z = tracking_data.rightFingers.thumb.joint1.v[2];
	BONE_THUMB_JOINT1_R_Pos = CoordinateUtil::GetVector3(BONE_THUMB_JOINT1_R_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_THUMB_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_R]->SetPosition(BONE_THUMB_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_R]->SetRotation(BONE_THUMB_JOINT1_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT1_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm thumb joint2 bone - BONE_THUMB_JOINT2_R.
	FVector BONE_THUMB_JOINT2_R_Pos = FVector::ZeroVector;
	BONE_THUMB_JOINT2_R_Pos.X = tracking_data.rightFingers.thumb.joint2.v[0];
	BONE_THUMB_JOINT2_R_Pos.Y = tracking_data.rightFingers.thumb.joint2.v[1];
	BONE_THUMB_JOINT2_R_Pos.Z = tracking_data.rightFingers.thumb.joint2.v[2];
	BONE_THUMB_JOINT2_R_Pos = CoordinateUtil::GetVector3(BONE_THUMB_JOINT2_R_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_THUMB_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_R]->SetPosition(BONE_THUMB_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_R]->SetRotation(BONE_THUMB_JOINT2_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT2_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm thumb joint2 bone - BONE_THUMB_JOINT3_R.
	FVector BONE_THUMB_JOINT3_R_Pos = FVector::ZeroVector;
	BONE_THUMB_JOINT3_R_Pos.X = tracking_data.rightFingers.thumb.joint3.v[0];
	BONE_THUMB_JOINT3_R_Pos.Y = tracking_data.rightFingers.thumb.joint3.v[1];
	BONE_THUMB_JOINT3_R_Pos.Z = tracking_data.rightFingers.thumb.joint3.v[2];
	BONE_THUMB_JOINT3_R_Pos = CoordinateUtil::GetVector3(BONE_THUMB_JOINT3_R_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT2_R_Pos, BONE_THUMB_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_R]->SetPosition(BONE_THUMB_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_R]->SetRotation(BONE_THUMB_JOINT3_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_JOINT3_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm thumb tip bone - BONE_THUMB_TIP_R.
	FVector BONE_THUMB_TIP_R_Pos = FVector::ZeroVector;
	BONE_THUMB_TIP_R_Pos.X = tracking_data.rightFingers.thumb.tip.v[0];
	BONE_THUMB_TIP_R_Pos.Y = tracking_data.rightFingers.thumb.tip.v[1];
	BONE_THUMB_TIP_R_Pos.Z = tracking_data.rightFingers.thumb.tip.v[2];
	BONE_THUMB_TIP_R_Pos = CoordinateUtil::GetVector3(BONE_THUMB_TIP_R_Pos, HMD->GetWorldToMetersScale());
	BONE_THUMB_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_THUMB_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_THUMB_JOINT3_R_Pos, BONE_THUMB_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_R]->SetPosition(BONE_THUMB_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_R]->SetRotation(BONE_THUMB_TIP_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_THUMB_TIP_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm index joint1 bone - BONE_INDEX_JOINT1_R.
	FVector BONE_INDEX_JOINT1_R_Pos = FVector::ZeroVector;
	BONE_INDEX_JOINT1_R_Pos.X = tracking_data.rightFingers.index.joint1.v[0];
	BONE_INDEX_JOINT1_R_Pos.Y = tracking_data.rightFingers.index.joint1.v[1];
	BONE_INDEX_JOINT1_R_Pos.Z = tracking_data.rightFingers.index.joint1.v[2];
	BONE_INDEX_JOINT1_R_Pos = CoordinateUtil::GetVector3(BONE_INDEX_JOINT1_R_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_INDEX_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_R]->SetPosition(BONE_INDEX_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_R]->SetRotation(BONE_INDEX_JOINT1_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT1_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm index joint2 bone - BONE_INDEX_JOINT2_R.
	FVector BONE_INDEX_JOINT2_R_Pos = FVector::ZeroVector;
	BONE_INDEX_JOINT2_R_Pos.X = tracking_data.rightFingers.index.joint2.v[0];
	BONE_INDEX_JOINT2_R_Pos.Y = tracking_data.rightFingers.index.joint2.v[1];
	BONE_INDEX_JOINT2_R_Pos.Z = tracking_data.rightFingers.index.joint2.v[2];
	BONE_INDEX_JOINT2_R_Pos = CoordinateUtil::GetVector3(BONE_INDEX_JOINT2_R_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT1_R_Pos, BONE_INDEX_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_R]->SetPosition(BONE_INDEX_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_R]->SetRotation(BONE_INDEX_JOINT2_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT2_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm index joint3 bone - BONE_INDEX_JOINT3_R.
	FVector BONE_INDEX_JOINT3_R_Pos = FVector::ZeroVector;
	BONE_INDEX_JOINT3_R_Pos.X = tracking_data.rightFingers.index.joint3.v[0];
	BONE_INDEX_JOINT3_R_Pos.Y = tracking_data.rightFingers.index.joint3.v[1];
	BONE_INDEX_JOINT3_R_Pos.Z = tracking_data.rightFingers.index.joint3.v[2];
	BONE_INDEX_JOINT3_R_Pos = CoordinateUtil::GetVector3(BONE_INDEX_JOINT3_R_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT2_R_Pos, BONE_INDEX_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_R]->SetPosition(BONE_INDEX_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_R]->SetRotation(BONE_INDEX_JOINT3_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_JOINT3_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm index tip bone - BONE_INDEX_TIP_R.
	FVector BONE_INDEX_TIP_R_Pos = FVector::ZeroVector;
	BONE_INDEX_TIP_R_Pos.X = tracking_data.rightFingers.index.tip.v[0];
	BONE_INDEX_TIP_R_Pos.Y = tracking_data.rightFingers.index.tip.v[1];
	BONE_INDEX_TIP_R_Pos.Z = tracking_data.rightFingers.index.tip.v[2];
	BONE_INDEX_TIP_R_Pos = CoordinateUtil::GetVector3(BONE_INDEX_TIP_R_Pos, HMD->GetWorldToMetersScale());
	BONE_INDEX_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_INDEX_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_INDEX_JOINT3_R_Pos, BONE_INDEX_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_R]->SetPosition(BONE_INDEX_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_R]->SetRotation(BONE_INDEX_TIP_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_INDEX_TIP_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm middle joint1 bone - BONE_MIDDLE_JOINT1_R.
	FVector BONE_MIDDLE_JOINT1_R_Pos = FVector::ZeroVector;
	BONE_MIDDLE_JOINT1_R_Pos.X = tracking_data.rightFingers.middle.joint1.v[0];
	BONE_MIDDLE_JOINT1_R_Pos.Y = tracking_data.rightFingers.middle.joint1.v[1];
	BONE_MIDDLE_JOINT1_R_Pos.Z = tracking_data.rightFingers.middle.joint1.v[2];
	BONE_MIDDLE_JOINT1_R_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_JOINT1_R_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_MIDDLE_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_R]->SetPosition(BONE_MIDDLE_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_R]->SetRotation(BONE_MIDDLE_JOINT1_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT1_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm middle joint2 bone - BONE_MIDDLE_JOINT2_R.
	FVector BONE_MIDDLE_JOINT2_R_Pos = FVector::ZeroVector;
	BONE_MIDDLE_JOINT2_R_Pos.X = tracking_data.rightFingers.middle.joint2.v[0];
	BONE_MIDDLE_JOINT2_R_Pos.Y = tracking_data.rightFingers.middle.joint2.v[1];
	BONE_MIDDLE_JOINT2_R_Pos.Z = tracking_data.rightFingers.middle.joint2.v[2];
	BONE_MIDDLE_JOINT2_R_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_JOINT2_R_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT1_R_Pos, BONE_MIDDLE_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_R]->SetPosition(BONE_MIDDLE_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_R]->SetRotation(BONE_MIDDLE_JOINT2_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT2_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm middle joint3 bone - BONE_MIDDLE_JOINT3_R.
	FVector BONE_MIDDLE_JOINT3_R_Pos = FVector::ZeroVector;
	BONE_MIDDLE_JOINT3_R_Pos.X = tracking_data.rightFingers.middle.joint3.v[0];
	BONE_MIDDLE_JOINT3_R_Pos.Y = tracking_data.rightFingers.middle.joint3.v[1];
	BONE_MIDDLE_JOINT3_R_Pos.Z = tracking_data.rightFingers.middle.joint3.v[2];
	BONE_MIDDLE_JOINT3_R_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_JOINT3_R_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT2_R_Pos, BONE_MIDDLE_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_R]->SetPosition(BONE_MIDDLE_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_R]->SetRotation(BONE_MIDDLE_JOINT3_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_JOINT3_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm middle tip bone - BONE_MIDDLE_TIP_R.
	FVector BONE_MIDDLE_TIP_R_Pos = FVector::ZeroVector;
	BONE_MIDDLE_TIP_R_Pos.X = tracking_data.rightFingers.middle.tip.v[0];
	BONE_MIDDLE_TIP_R_Pos.Y = tracking_data.rightFingers.middle.tip.v[1];
	BONE_MIDDLE_TIP_R_Pos.Z = tracking_data.rightFingers.middle.tip.v[2];
	BONE_MIDDLE_TIP_R_Pos = CoordinateUtil::GetVector3(BONE_MIDDLE_TIP_R_Pos, HMD->GetWorldToMetersScale());
	BONE_MIDDLE_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_MIDDLE_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_MIDDLE_JOINT3_R_Pos, BONE_MIDDLE_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_R]->SetPosition(BONE_MIDDLE_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_R]->SetRotation(BONE_MIDDLE_TIP_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_MIDDLE_TIP_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm ring joint1 bone - BONE_RING_JOINT1_R.
	FVector BONE_RING_JOINT1_R_Pos = FVector::ZeroVector;
	BONE_RING_JOINT1_R_Pos.X = tracking_data.rightFingers.ring.joint1.v[0];
	BONE_RING_JOINT1_R_Pos.Y = tracking_data.rightFingers.ring.joint1.v[1];
	BONE_RING_JOINT1_R_Pos.Z = tracking_data.rightFingers.ring.joint1.v[2];
	BONE_RING_JOINT1_R_Pos = CoordinateUtil::GetVector3(BONE_RING_JOINT1_R_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_RING_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_R]->SetPosition(BONE_RING_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_R]->SetRotation(BONE_RING_JOINT1_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT1_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm ring joint2 bone - BONE_RING_JOINT2_R.
	FVector BONE_RING_JOINT2_R_Pos = FVector::ZeroVector;
	BONE_RING_JOINT2_R_Pos.X = tracking_data.rightFingers.ring.joint2.v[0];
	BONE_RING_JOINT2_R_Pos.Y = tracking_data.rightFingers.ring.joint2.v[1];
	BONE_RING_JOINT2_R_Pos.Z = tracking_data.rightFingers.ring.joint2.v[2];
	BONE_RING_JOINT2_R_Pos = CoordinateUtil::GetVector3(BONE_RING_JOINT2_R_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT1_R_Pos, BONE_RING_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_R]->SetPosition(BONE_RING_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_R]->SetRotation(BONE_RING_JOINT2_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT2_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm ring joint3 bone - BONE_RING_JOINT3_R.
	FVector BONE_RING_JOINT3_R_Pos = FVector::ZeroVector;
	BONE_RING_JOINT3_R_Pos.X = tracking_data.rightFingers.ring.joint3.v[0];
	BONE_RING_JOINT3_R_Pos.Y = tracking_data.rightFingers.ring.joint3.v[1];
	BONE_RING_JOINT3_R_Pos.Z = tracking_data.rightFingers.ring.joint3.v[2];
	BONE_RING_JOINT3_R_Pos = CoordinateUtil::GetVector3(BONE_RING_JOINT3_R_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT2_R_Pos, BONE_RING_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_R]->SetPosition(BONE_RING_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_R]->SetRotation(BONE_RING_JOINT3_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_JOINT3_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm ring tip bone - BONE_RING_TIP_R.
	FVector BONE_RING_TIP_R_Pos = FVector::ZeroVector;
	BONE_RING_TIP_R_Pos.X = tracking_data.rightFingers.ring.tip.v[0];
	BONE_RING_TIP_R_Pos.Y = tracking_data.rightFingers.ring.tip.v[1];
	BONE_RING_TIP_R_Pos.Z = tracking_data.rightFingers.ring.tip.v[2];
	BONE_RING_TIP_R_Pos = CoordinateUtil::GetVector3(BONE_RING_TIP_R_Pos, HMD->GetWorldToMetersScale());
	BONE_RING_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_RING_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_RING_JOINT3_R_Pos, BONE_RING_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_R]->SetPosition(BONE_RING_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_R]->SetRotation(BONE_RING_TIP_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_RING_TIP_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm pinky joint1 bone - BONE_PINKY_JOINT1_R.
	FVector BONE_PINKY_JOINT1_R_Pos = FVector::ZeroVector;
	BONE_PINKY_JOINT1_R_Pos.X = tracking_data.rightFingers.pinky.joint1.v[0];
	BONE_PINKY_JOINT1_R_Pos.Y = tracking_data.rightFingers.pinky.joint1.v[1];
	BONE_PINKY_JOINT1_R_Pos.Z = tracking_data.rightFingers.pinky.joint1.v[2];
	BONE_PINKY_JOINT1_R_Pos = CoordinateUtil::GetVector3(BONE_PINKY_JOINT1_R_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT1_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_JOINT1_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_HAND_WRIST_R_Pos, BONE_PINKY_JOINT1_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_R]->SetPosition(BONE_PINKY_JOINT1_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_R]->SetRotation(BONE_PINKY_JOINT1_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT1_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm pinky joint2 bone - BONE_PINKY_JOINT2_R.
	FVector BONE_PINKY_JOINT2_R_Pos = FVector::ZeroVector;
	BONE_PINKY_JOINT2_R_Pos.X = tracking_data.rightFingers.pinky.joint2.v[0];
	BONE_PINKY_JOINT2_R_Pos.Y = tracking_data.rightFingers.pinky.joint2.v[1];
	BONE_PINKY_JOINT2_R_Pos.Z = tracking_data.rightFingers.pinky.joint2.v[2];
	BONE_PINKY_JOINT2_R_Pos = CoordinateUtil::GetVector3(BONE_PINKY_JOINT2_R_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT2_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_JOINT2_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT1_R_Pos, BONE_PINKY_JOINT2_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_R]->SetPosition(BONE_PINKY_JOINT2_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_R]->SetRotation(BONE_PINKY_JOINT2_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT2_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm pinky joint3 bone - BONE_PINKY_JOINT3_R.
	FVector BONE_PINKY_JOINT3_R_Pos = FVector::ZeroVector;
	BONE_PINKY_JOINT3_R_Pos.X = tracking_data.rightFingers.pinky.joint3.v[0];
	BONE_PINKY_JOINT3_R_Pos.Y = tracking_data.rightFingers.pinky.joint3.v[1];
	BONE_PINKY_JOINT3_R_Pos.Z = tracking_data.rightFingers.pinky.joint3.v[2];
	BONE_PINKY_JOINT3_R_Pos = CoordinateUtil::GetVector3(BONE_PINKY_JOINT3_R_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_JOINT3_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_JOINT3_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT2_R_Pos, BONE_PINKY_JOINT3_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_R]->SetPosition(BONE_PINKY_JOINT3_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_R]->SetRotation(BONE_PINKY_JOINT3_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_JOINT3_R]->SetPoseValid(tracking_data.right.isValidPose);

	// Right arm pinky tip bone - BONE_PINKY_TIP_R.
	FVector BONE_PINKY_TIP_R_Pos = FVector::ZeroVector;
	BONE_PINKY_TIP_R_Pos.X = tracking_data.rightFingers.pinky.tip.v[0];
	BONE_PINKY_TIP_R_Pos.Y = tracking_data.rightFingers.pinky.tip.v[1];
	BONE_PINKY_TIP_R_Pos.Z = tracking_data.rightFingers.pinky.tip.v[2];
	BONE_PINKY_TIP_R_Pos = CoordinateUtil::GetVector3(BONE_PINKY_TIP_R_Pos, HMD->GetWorldToMetersScale());
	BONE_PINKY_TIP_R_Pos += BONE_OFFSET_R;
	FRotator BONE_PINKY_TIP_R_Rot = UKismetMathLibrary::FindLookAtRotation(BONE_PINKY_JOINT3_R_Pos, BONE_PINKY_TIP_R_Pos);

	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_R]->SetPosition(BONE_PINKY_TIP_R_Pos);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_R]->SetRotation(BONE_PINKY_TIP_R_Rot);
	Skeleton->Bones[(int32)EWaveVRGestureBoneType::BONE_PINKY_TIP_R]->SetPoseValid(tracking_data.right.isValidPose);

}
#pragma endregion Hand Tracking

#pragma region
void FWaveVRGestureInputDevice::UpdateLeftHandGestureData(WVR_HandGestureData gesture_data)
{
	// Update the left gesture.
	prevStaticGestureLeft = currStaticGestureLeft;
	currStaticGestureLeft = EWaveVRStaticGesture::TYPE_INVALID;

	if (gesture_data.left == WVR_HandGestureType::WVR_HandGestureType_Unknown)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_UNKNOWN;
	if (gesture_data.left == WVR_HandGestureType::WVR_HandGestureType_Fist)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_FIST;
	if (gesture_data.left == WVR_HandGestureType::WVR_HandGestureType_Five)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_FIVE;
	if (gesture_data.left == WVR_HandGestureType::WVR_HandGestureType_OK)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_OK;
	if (gesture_data.left == WVR_HandGestureType::WVR_HandGestureType_ThumbUp)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_LIKE;
	if (gesture_data.left == WVR_HandGestureType::WVR_HandGestureType_IndexUp)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_POINT;
	if (gesture_data.left == WVR_HandGestureType::WVR_HandGestureType_Pinch)
		currStaticGestureLeft = EWaveVRStaticGesture::TYPE_PINCH;

	if (currStaticGestureLeft != prevStaticGestureLeft)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("UpdateLeftHandGestureData() left static gesture %d"), (uint8)currStaticGestureLeft);
		UWaveVRStaticGestureComponent::OnStaticGestureNative_Left.Broadcast(currStaticGestureLeft);
	}
}

void FWaveVRGestureInputDevice::UpdateRightHandGestureData(WVR_HandGestureData gesture_data)
{
	// Update the right gesture.
	prevStaticGestureRight = currStaticGestureRight;
	currStaticGestureRight = EWaveVRStaticGesture::TYPE_INVALID;

	if (gesture_data.right == WVR_HandGestureType::WVR_HandGestureType_Unknown)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_UNKNOWN;
	if (gesture_data.right == WVR_HandGestureType::WVR_HandGestureType_Fist)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_FIST;
	if (gesture_data.right == WVR_HandGestureType::WVR_HandGestureType_Five)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_FIVE;
	if (gesture_data.right == WVR_HandGestureType::WVR_HandGestureType_OK)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_OK;
	if (gesture_data.right == WVR_HandGestureType::WVR_HandGestureType_ThumbUp)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_LIKE;
	if (gesture_data.right == WVR_HandGestureType::WVR_HandGestureType_IndexUp)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_POINT;
	if (gesture_data.right == WVR_HandGestureType::WVR_HandGestureType_Pinch)
		currStaticGestureRight = EWaveVRStaticGesture::TYPE_PINCH;

	if (currStaticGestureRight != prevStaticGestureRight)
	{
		UE_LOG(LogWaveVRGestureInputDevice, Log, TEXT("UpdateRightHandGestureData() right static gesture %d"), (uint8)currStaticGestureRight);
		UWaveVRStaticGestureComponent::OnStaticGestureNative_Right.Broadcast(currStaticGestureRight);
	}
}
#pragma endregion Hand Gesture

#pragma region
void FWaveVRGestureInputDevice::UpdateParametersFromPawn()
{
	if (!World)
		World = GWorld->GetWorld();
	if (World)
	{
		FString WaveVRPawnName = FString(TEXT("WaveVR_Pawn_C"));
		bool IsWaveVRPawnExist = false;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->IsPendingKill())
			{
				FString str = Actor->GetClass()->GetName();

				if (str == WaveVRPawnName) {
					IsWaveVRPawnExist = true;
					FName CameraHeight = FName(TEXT("CameraHeight"));
					UProperty* Property = Actor->GetClass()->FindPropertyByName(CameraHeight);
					if (Property)
					{
						FVector* ptrValue = Property->ContainerPtrToValuePtr<FVector>(Actor);
						if (ptrValue)
						{
							BONE_OFFSET_L = *ptrValue;
							BONE_OFFSET_R = *ptrValue;
						}
					}
				}
			}
		}
	}
}
#pragma endregion WaveVR Pawn Related Interface
