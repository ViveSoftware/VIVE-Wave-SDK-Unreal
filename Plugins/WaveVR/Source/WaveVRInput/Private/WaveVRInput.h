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
#include "IWaveVRInputModule.h"
#include "WaveVRHMD.h"
#include "WaveVRController.h"
#include "WaveVRInputSimulator.h"

#include "GenericPlatform/IInputInterface.h"
#include "XRMotionControllerBase.h"
#include "IHapticDevice.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRInput, Log, All);

#define WAVEVR_DEBUG false
#define LOCTEXT_NAMESPACE "WaveVR Unreal"
//-------------------------------------------------------------------------------------------------
// FWaveVRInput
//-------------------------------------------------------------------------------------------------

namespace WaveVRControllerKeyNames
{
	const FGamepadKeyNames::Type AndroidVolumeUp	("Android_Volume_Up");
	const FGamepadKeyNames::Type AndroidVolumeDown	("Android_Volume_Down");

	const FGamepadKeyNames::Type LeftTouchpadTouch	("Steam_Touch_0");
	const FGamepadKeyNames::Type RightTouchpadTouch	("Steam_Touch_1");
	const FGamepadKeyNames::Type LeftTriggerTouch	("Steam_Touch_2");
	const FGamepadKeyNames::Type RightTriggerTouch	("Steam_Touch_3");

	static const FKey LeftJoystickPress("WVRLeftJoystickPress");
	static const FKey RightJoystickPress("WVRRightJoystickPress");

	static const FKey LeftJoystickTouch("WVRLeftJoystickTouch");
	static const FKey LeftJoystickAxisX("WVRLeftJoystickAxisX");
	static const FKey LeftJoystickAxisY("WVRLeftJoystickAxisY");

	static const FKey RightJoystickTouch("WVRRightJoystickTouch");
	static const FKey RightJoystickAxisX("WVRRightJoystickAxisX");
	static const FKey RightJoystickAxisY("WVRRightJoystickAxisY");

	static const FKey HmdBackPress("WVRHmdBackPress");
	static const FKey HmdEnterPress("WVRHmdEnterPress");
}


class FWaveVRInput : public IInputDevice, public FXRMotionControllerBase, public IHapticDevice
{

	FWaveVRHMD* GetWaveVRHMD() const {
		if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetSystemName() == TEXT("WaveVR"))
		{
			return static_cast<FWaveVRHMD*>(GEngine->XRSystem->GetHMDDevice());
		}
		return nullptr;
	}
public:

	/** Constructor that takes an initial message handler that will receive motion controller events */
	FWaveVRInput(WaveVRInputSimulator* simulator, const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler );

	/** Clean everything up */
	virtual ~FWaveVRInput();
	bool IsInputInitialized();

	static const int ControllerCount = 2;
	FGamepadKeyNames::Type ControllerPressButtons[ControllerCount][InputButtonCount];
	bool PrevPressState[ControllerCount][InputButtonCount];
	FGamepadKeyNames::Type ControllerTouchButtons[ControllerCount][TouchButtonCount];
	bool PrevTouchState[ControllerCount][TouchButtonCount];

	void EnableInputSimulator(UObject * WorldContextObject);

private:
	void fireButtonPressEvent(FName button_name, bool down);
	void fireAllButtonPressEvent(EControllerHand hand, EWVR_InputId id, bool down);
	WaveVRInputSimulator * pSimulator;
	bool bInputInitialized = false;

public:	// IInputDevice overrides
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override
	{
		return false;
	}
	virtual void SendControllerEvents() override;
	/**
	 * IForceFeedbackSystem pass through functions
	 * This class is deprecated and will be removed in favor of IInputInterface
	 */
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override {}
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues& Values) override {}

	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override
	{
		MessageHandler = InMessageHandler;
	}
	virtual void Tick(float DeltaTime) override;

private:
	void UpdateButtonPressStates(EControllerHand hand);
	void UpdateButtonTouchStates(EControllerHand hand);

	float fFPS;

public:	// IMotionController overrides
	static FName DeviceTypeName;
	virtual FName GetMotionControllerDeviceTypeName() const override
	{
		return DeviceTypeName;
	}
	virtual bool GetControllerOrientationAndPosition( const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale ) const override;
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const override;

public:	// IHapticDevice overrides
	IHapticDevice* GetHapticDevice() override { return (IHapticDevice*)this; }
	virtual void SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) override;

	virtual void GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const override;
	virtual float GetHapticAmplitudeScale() const override;

public:	// Real & Simulation Pose
	struct RigidTransform {
		FVector pos;	// position
		FQuat rot;		// rotation
	};

	struct Transform {
		FVector localPosition;
		FRotator localRotation;
	};

	void UseSimulationPose(SimulatePosition simulation);
	void FollowHeadPosition(bool follow);

	void UpdateUnitySimulationSettingsFromJson(
		FVector HEADTOELBOW_OFFSET,
		FVector ELBOWTOWRIST_OFFSET,
		FVector WRISTTOCONTROLLER_OFFSET,
		FVector ELBOW_PITCH_OFFSET,
		float ELBOW_PITCH_ANGLE_MIN,
		float ELBOW_PITCH_ANGLE_MAX
		);

private:// Real & Simulation Pose
	void UpdatePose();
	void UpdateControllerPose(EWVR_DeviceType hand);
	FVector GetHeadUEPosition();
	FVector GetHeadUnityPosition();
	FQuat GetHeadUnityRotation();
	FVector GetHeadUnityForward();
	void UpdateHeadAndBodyPose(EWVR_DeviceType hand);
	float BodyRotationFilter(RigidTransform pre_pose, RigidTransform pose);
	void ComputeControllerUnityPose(RigidTransform rtPose);

	RigidTransform unityPose[EWVR_DeviceType_Count];
	RigidTransform unityPose_prev[EWVR_DeviceType_Count];
	Transform uePose[EWVR_DeviceType_Count];
	Transform uePose_pev[EWVR_DeviceType_Count];
	bool bPoseIsValid[EWVR_DeviceType_Count];

	EWVR_DOF CurrentDoF[EWVR_DeviceType_Count];
	SimulatePosition enumUseSimulationPose;
	const float BodyAngleBound = 0.01f;
	const float BodyAngleLimitation = 0.3f;	// bound of controller angle in SPEC provided to provider.

	FVector UNITY_HEADTOELBOW_OFFSET = FVector(0.2f, -0.7f, 0);
	FVector UNITY_ELBOWTOWRIST_OFFSET = FVector(0.0f, 0.0f, 0.15f);
	FVector UNITY_WRISTTOCONTROLLER_OFFSET = FVector(0.0f, 0.0f, 0.05f);
	FVector UNITY_ELBOW_PITCH_OFFSET = FVector(-0.2, 0.55f, 0.08f);
	float UNITY_ELBOW_PITCH_ANGLE_MIN = 0;
	float UNITY_ELBOW_PITCH_ANGLE_MAX = 90;
	const float ELBOW_TO_XYPLANE_LERP_MIN = 0.45f;
	const float ELBOW_TO_XYPLANE_LERP_MAX = 0.65f;

	const FVector UNITY_FORWARD = FVector(0, 0, 1);
	const FVector UNITY_RIGHT = FVector(1, 0, 0);
	const FVector UNITY_UP = FVector(0, 1, 0);

	unsigned int framesOfFreeze;	// if framesOfFreeze >= fFPS, means controller freezed.
	bool FollowHead;
	FVector defaultHeadUEPosition;
	FVector defaultHeadUnityPosition;
	FVector bodyDirection;
	FQuat bodyRotation;
	FVector simulateUnityPosition;
	FVector simulateUEPosition;
	FQuat simulateUnityQuaternion;
	FRotator simulateUERotation;
	FVector v3ChangeArmYAxis;

public:
	bool IsLeftHandedMode();
private:
	bool bIsLeftHanded;
	EWVR_DeviceType GetLeftHandedDevice(EControllerHand hand);
	//FWaveVRHMD* HmdPlugin = FWaveVRHMD::GetInstance();

	/** The recipient of motion controller input events */
	TSharedPtr< FGenericApplicationMessageHandler > MessageHandler;
};
