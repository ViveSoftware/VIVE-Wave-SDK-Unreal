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
#include "WaveVRHMD.h"
#include "WaveVRGestureEnums.h"
#include "WaveVRGestureUtils.h"
#include "FWaveVRGestureThread.h"

#include "IInputInterface.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRGestureInputDevice, Log, All);

struct WaveVRGestureBoneData
{
	FVector Position;
	FRotator Rotation;
	bool Valid;

	WaveVRGestureBoneData()
		: Position(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
		, Valid(false)
	{
	}
};

class WaveVRGestureBone
{
public:
	FString Name;
	WaveVRGestureBoneData BoneData;

	FVector GetPosition() { return BoneData.Position; }
	void SetPosition(const FVector& InPosition) { BoneData.Position = InPosition; }
	FRotator GetRotation() { return BoneData.Rotation; }
	void SetRotation(const FRotator& InRotation) { BoneData.Rotation = InRotation; }
	bool IsValidPose() { return BoneData.Valid; }
	void SetPoseValid(const bool& InValid) { BoneData.Valid = InValid; }

	WaveVRGestureBone(const FString BoneName) {
		Name = BoneName;
	}
};

class WaveVRGestureFinger
{
public:
	FString Name;
	WaveVRGestureBone* Joint1;
	WaveVRGestureBone* Joint2;
	WaveVRGestureBone* Joint3;
	WaveVRGestureBone* Tip;

	WaveVRGestureFinger(const FString FingerName) {
		Name = FingerName;
		FString BoneName;
		BoneName = FingerName + FString("_Joint1");
		Joint1 = new WaveVRGestureBone(BoneName);
		BoneName = FingerName + FString("_Joint2");
		Joint2 = new WaveVRGestureBone(BoneName);
		BoneName = FingerName + FString("_Joint3");
		Joint3 = new WaveVRGestureBone(BoneName);
		BoneName = FingerName + FString("_Tip");
		Tip = new WaveVRGestureBone(BoneName);
	}
};

class WaveVRGestureHand
{
public:
	FString Name;
	WaveVRGestureBone* Wrist;
	WaveVRGestureBone* Palm;
	WaveVRGestureFinger* Thumb;
	WaveVRGestureFinger* Index;
	WaveVRGestureFinger* Middle;
	WaveVRGestureFinger* Ring;
	WaveVRGestureFinger* Pinky;

	WaveVRGestureHand(const FString HandName) {
		Name = HandName;
		FString BoneName;
		BoneName = HandName + FString("_Wrist");
		Wrist = new WaveVRGestureBone(BoneName);
		BoneName = HandName + FString("_Palm");
		Palm = new WaveVRGestureBone(BoneName);
		FString FingerName;
		FingerName = HandName + FString("_Thumb");
		Thumb = new WaveVRGestureFinger(FingerName);
		FingerName = HandName + FString("_Index");
		Index = new WaveVRGestureFinger(FingerName);
		FingerName = HandName + FString("_Middle");
		Middle = new WaveVRGestureFinger(FingerName);
		FingerName = HandName + FString("_Ring");
		Ring = new WaveVRGestureFinger(FingerName);
		FingerName = HandName + FString("_Pinky");
		Pinky = new WaveVRGestureFinger(FingerName);
	}
};

class WaveVRGestureArm
{
public:
	FString Name;
	WaveVRGestureHand* Hand;
	WaveVRGestureBone* ForeArm;
	WaveVRGestureBone* UpperArm;

	WaveVRGestureArm(const FString ArmName) {
		Name = ArmName;
		FString HandName = ArmName + FString("_Hand");
		Hand = new WaveVRGestureHand(HandName);
		FString BoneName;
		BoneName = ArmName + FString("_ForeArm");
		ForeArm = new WaveVRGestureBone(BoneName);
		BoneName = ArmName + FString("_UpperArm");
		UpperArm = new WaveVRGestureBone(BoneName);
	}
};

class WaveVRGestureSkeleton
{
public:
	TArray<WaveVRGestureBone*> Bones;

	WaveVRGestureSkeleton() {
		for (int i = 0; i < (int32)EWaveVRGestureBoneType::BONES_COUNT; i++)
		{
			const FString BoneName = FWaveVRGestureUtils::EnumToString(TEXT("EWaveVRGestureBoneType"), (EWaveVRGestureBoneType)i);
			WaveVRGestureBone* Bone = new WaveVRGestureBone(BoneName);
			Bones.Add(Bone);
		}
	}
};

class FWaveVRGestureInputDevice : public IInputDevice
{
	FWaveVRHMD* GetWaveVRHMD() const {
		if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetSystemName() == TEXT("WaveVR"))
		{
			return static_cast<FWaveVRHMD*>(GEngine->XRSystem->GetHMDDevice());
		}
		return nullptr;
	}
public: // IInputDevice overrides.
	FWaveVRGestureInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	~FWaveVRGestureInputDevice();

	/** Tick the interface (e.g. check for new controllers) */
	virtual void Tick(float DeltaTime) override;

	/** Poll for controller state and send events if needed */
	virtual void SendControllerEvents() override;

	/** Set which MessageHandler will get the events from SendControllerEvents. */
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;

	/** Exec handler to allow console commands to be passed through for debugging */
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	/** IForceFeedbackSystem pass through functions **/
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;

private:
	/* Message handler */
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

public:
	bool IsGestureInputDeviceInitialized();

	void StartHandGesture();
	void StopHandGesture();
	bool IsHandGestureAvailable();
	EWaveVRStaticGesture GetStaticGestureType(EWaveVRGestureHandType DevType);
	EWaveVRHandGestureStatus GetHandGestureStatus();

	void StartHandTracking();
	void StopHandTracking();
	bool IsHandTrackingAvailable();
	bool GetBonePositionAndRotation(int32 DevId, EWaveVRGestureBoneType BoneId, FVector& OutPosition, FRotator& OutRotation);
	EWaveVRHandTrackingStatus GetHandTrackingStatus();

private:
	int32 DeviceIndex;
	class WaveVRGestureSkeleton* Skeleton;

	bool bGestureInputDeviceInitialized;

	WVR_HandGestureData handGestureData;
	bool bHandGestureDataUpdated;
	EWaveVRStaticGesture prevStaticGestureRight, prevStaticGestureLeft;
	EWaveVRStaticGesture currStaticGestureRight, currStaticGestureLeft;
	void UpdateLeftHandGestureData(WVR_HandGestureData handGestureData);
	void UpdateRightHandGestureData(WVR_HandGestureData handGestureData);

	WVR_HandTrackingData handTrackingData;
	bool bHandTrackingDataUpdated;
	void UpdateLeftHandTrackingData(WVR_HandTrackingData handTrackingData);
	void UpdateRightHandTrackingData(WVR_HandTrackingData handTrackingData);

private:	// FWaveVRGestureThread related.
	FWaveVRGestureThread * gestureThread;

private:	// WaveVR_Pawn related.
	UWorld* World;
	FVector BONE_OFFSET_L;
	FVector BONE_OFFSET_R;
	void UpdateParametersFromPawn();
};
