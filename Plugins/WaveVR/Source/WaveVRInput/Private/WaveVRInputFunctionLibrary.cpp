// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRInputFunctionLibrary.h"
#include "WaveVRInput.h"
#include "WaveVREventCommon.h"
#include "WaveVRPrivatePCH.h"

FWaveVRInput* GetWaveVRInput()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		if (MotionController != nullptr && MotionController->GetMotionControllerDeviceTypeName() == FWaveVRInput::DeviceTypeName)
		{
			return static_cast<FWaveVRInput*>(MotionController);
		}
	}

	return nullptr;
}

static EControllerHand EWVRDeviceTypeToEControllerHand(EWVR_DeviceType device)
{
	bool _lefthanded = UWaveVREventCommon::IsLeftHandedMode();
	switch (device)
	{
	case EWVR_DeviceType::DeviceType_HMD:
		return EControllerHand::ExternalCamera;
		break;
	case EWVR_DeviceType::DeviceType_Controller_Right:
		return _lefthanded ? EControllerHand::Left : EControllerHand::Right;
		break;
	case EWVR_DeviceType::DeviceType_Controller_Left:
		return _lefthanded ? EControllerHand::Right : EControllerHand::Left;
		break;
	default:
		break;
	}

	return EControllerHand::AnyHand;
}

static EWVR_DeviceType GetLeftHandedDevice(EWVR_DeviceType device)
{
	bool _lefthanded = UWaveVREventCommon::IsLeftHandedMode();

	EWVR_DeviceType _device = device;
	if (_lefthanded)
	{
		switch (device)
		{
		case EWVR_DeviceType::DeviceType_Controller_Right:
			_device = EWVR_DeviceType::DeviceType_Controller_Left;
			break;
		case EWVR_DeviceType::DeviceType_Controller_Left:
			_device = EWVR_DeviceType::DeviceType_Controller_Right;
			break;
		default:
			break;
		}
	}
	return _device;
}

void UWaveVRInputFunctionLibrary::InitializeWaveVRInput()
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
}

bool UWaveVRInputFunctionLibrary::IsWaveVRInputInitialized()
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		return WaveVRInput->IsInputInitialized();
	}

	return false;
}

FRotator UWaveVRInputFunctionLibrary::GetWaveVRInputDeviceOrientation(EWVR_DeviceType device)
{
	EControllerHand _hand = EWVRDeviceTypeToEControllerHand(device);
	if (_hand == EControllerHand::AnyHand)
		return FRotator::ZeroRotator;

	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		FRotator orientation;
		FVector position;

		bool bIsValid = WaveVRInput->GetControllerOrientationAndPosition(0, _hand, orientation, position, 0);
		return orientation; // no matter pose is invalid or not, return pose.
	}

	return FRotator::ZeroRotator;
}

FVector UWaveVRInputFunctionLibrary::GetWaveVRInputDevicePosition(EWVR_DeviceType device)
{
	EControllerHand _hand = EWVRDeviceTypeToEControllerHand(device);
	if (_hand == EControllerHand::AnyHand)
		return FVector::ZeroVector;

	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		FRotator orientation;
		FVector position;
		bool bIsValid = WaveVRInput->GetControllerOrientationAndPosition(0, _hand, orientation, position, 0);
		return position; // no matter pose is invalid or not, return pose.
	}

	return FVector::ZeroVector;
}

ETrackingStatus UWaveVRInputFunctionLibrary::GetWaveVRInputDeviceTrackingStatus(EWVR_DeviceType device)
{
	EControllerHand _hand = EWVRDeviceTypeToEControllerHand(device);
	if (_hand == EControllerHand::AnyHand)
		return ETrackingStatus::NotTracked;

	ETrackingStatus status = ETrackingStatus::NotTracked;
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		status = WaveVRInput->GetControllerTrackingStatus((int32)_hand, _hand);
	}

	return status;
}

FVector UWaveVRInputFunctionLibrary::GetWaveVRInputDeviceVelocity(EWVR_DeviceType device)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	FVector velocity = UWaveVRBlueprintFunctionLibrary::GetDeviceVelocity(_device);

	return velocity;
}

FVector UWaveVRInputFunctionLibrary::GetWaveVRInputDeviceAngularVelocity(EWVR_DeviceType device)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	FVector angularv = UWaveVRBlueprintFunctionLibrary::GetDeviceAngularVelocity(_device);

	return angularv;
}

void UWaveVRInputFunctionLibrary::UseSimulationPose(SimulatePosition simulation)
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		WaveVRInput->UseSimulationPose(simulation);
	}
}

void UWaveVRInputFunctionLibrary::EnableInputSimulator(UObject * WorldContextObject)
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		WaveVRInput->EnableInputSimulator(WorldContextObject);
	}
}

void UWaveVRInputFunctionLibrary::FollowHeadPosition(bool follow)
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		WaveVRInput->FollowHeadPosition(follow);
	}
}

void UWaveVRInputFunctionLibrary::UpdateUnitySimulationSettingsFromJson(
	FVector HEADTOELBOW_OFFSET,
	FVector ELBOWTOWRIST_OFFSET,
	FVector WRISTTOCONTROLLER_OFFSET,
	FVector ELBOW_PITCH_OFFSET,
	float ELBOW_PITCH_ANGLE_MIN,
	float ELBOW_PITCH_ANGLE_MAX)
{
	FWaveVRInput* WaveVRInput = GetWaveVRInput();
	if (WaveVRInput != nullptr)
	{
		WaveVRInput->UpdateUnitySimulationSettingsFromJson(
			HEADTOELBOW_OFFSET,
			ELBOWTOWRIST_OFFSET,
			WRISTTOCONTROLLER_OFFSET,
			ELBOW_PITCH_OFFSET,
			ELBOW_PITCH_ANGLE_MIN,
			ELBOW_PITCH_ANGLE_MAX);
	}
}


bool UWaveVRInputFunctionLibrary::IsInputAvailable(EWVR_DeviceType device)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	return UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(_device);
}

bool UWaveVRInputFunctionLibrary::IsInputPoseValid(EWVR_DeviceType device)
{
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.World()->IsPlayInEditor())
		{
			return true;
		}
	}
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	FVector _position = FVector::ZeroVector;
	FRotator _rotation = FRotator::ZeroRotator;

	return UWaveVRBlueprintFunctionLibrary::GetDevicePose(_position, _rotation, _device);
}

bool UWaveVRInputFunctionLibrary::IsInputButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	//return UWaveVRController::IsControllerButtonPressed(_device, button_id);
	return UWaveVREventCommon::IsControllerButtonPressed(_device, button_id);
}

bool UWaveVRInputFunctionLibrary::IsInputButtonTouched(EWVR_DeviceType device, EWVR_TouchId button_id)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	//return UWaveVRController::IsControllerButtonTouched(_device, button_id);
	return UWaveVREventCommon::IsControllerButtonTouched(_device, button_id);
}

EWVR_InputId UWaveVRInputFunctionLibrary::GetInputMappingPair(EWVR_DeviceType type, EWVR_InputId button)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(type);

	return UWaveVRBlueprintFunctionLibrary::GetInputMappingPair(_device, button);
}

bool UWaveVRInputFunctionLibrary::IsButtonAvailable(EWVR_DeviceType type, EWVR_InputId button)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(type);

	return UWaveVRBlueprintFunctionLibrary::IsButtonAvailable(_device, button);
}

FVector2D UWaveVRInputFunctionLibrary::GetInputButtonAxis(EWVR_DeviceType device, EWVR_TouchId button_id)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	return UWaveVRController::GetControllerAxis(_device, button_id);
}

float UWaveVRInputFunctionLibrary::GetDeviceBatteryPercentage(EWVR_DeviceType device)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	return UWaveVRBlueprintFunctionLibrary::getDeviceBatteryPercentage(_device);
}

bool UWaveVRInputFunctionLibrary::SetInteractionMode(EWVR_InteractionMode mode) {
	switch (mode) {
	case EWVR_InteractionMode::InteractionMode_SystemDefault:
		return FWaveVRAPIWrapper::GetInstance()->SetInteractionMode(WVR_InteractionMode::WVR_InteractionMode_SystemDefault);
	case EWVR_InteractionMode::InteractionMode_Gaze:
		return FWaveVRAPIWrapper::GetInstance()->SetInteractionMode(WVR_InteractionMode::WVR_InteractionMode_Gaze);
	case EWVR_InteractionMode::InteractionMode_Controller:
		return FWaveVRAPIWrapper::GetInstance()->SetInteractionMode(WVR_InteractionMode::WVR_InteractionMode_Controller);
	default:
		return false;
	}
}

EWVR_InteractionMode UWaveVRInputFunctionLibrary::GetInteractionMode() {
	WVR_InteractionMode mode = FWaveVRAPIWrapper::GetInstance()->GetInteractionMode();
	switch (mode) {
	case WVR_InteractionMode::WVR_InteractionMode_SystemDefault:
		return EWVR_InteractionMode::InteractionMode_SystemDefault;
	case WVR_InteractionMode::WVR_InteractionMode_Gaze:
		return EWVR_InteractionMode::InteractionMode_Gaze;
	case WVR_InteractionMode::WVR_InteractionMode_Controller:
		return EWVR_InteractionMode::InteractionMode_Controller;
	default:
		return EWVR_InteractionMode::InteractionMode_SystemDefault;
	}
}

void UWaveVRInputFunctionLibrary::SetGazeTriggerType(EWVR_GazeTriggerType type) {
	switch (type) {
	case EWVR_GazeTriggerType::GazeTriggerType_Timeout:
		FWaveVRAPIWrapper::GetInstance()->SetGazeTriggerType(WVR_GazeTriggerType::WVR_GazeTriggerType_Timeout);
	case EWVR_GazeTriggerType::GazeTriggerType_Button:
		FWaveVRAPIWrapper::GetInstance()->SetGazeTriggerType(WVR_GazeTriggerType::WVR_GazeTriggerType_Button);
	case EWVR_GazeTriggerType::GazeTriggerType_TimeoutButton:
		FWaveVRAPIWrapper::GetInstance()->SetGazeTriggerType(WVR_GazeTriggerType::WVR_GazeTriggerType_TimeoutButton);
	default:
		FWaveVRAPIWrapper::GetInstance()->SetGazeTriggerType(WVR_GazeTriggerType::WVR_GazeTriggerType_Timeout);
	}
}

EWVR_GazeTriggerType UWaveVRInputFunctionLibrary::GetGazeTriggerType() {
	WVR_GazeTriggerType type = FWaveVRAPIWrapper::GetInstance()->GetGazeTriggerType();
	switch (type) {
	case WVR_GazeTriggerType::WVR_GazeTriggerType_Timeout:
		return EWVR_GazeTriggerType::GazeTriggerType_Timeout;
	case WVR_GazeTriggerType::WVR_GazeTriggerType_Button:
		return EWVR_GazeTriggerType::GazeTriggerType_Button;
	case WVR_GazeTriggerType::WVR_GazeTriggerType_TimeoutButton:
		return EWVR_GazeTriggerType::GazeTriggerType_TimeoutButton;
	default:
		return EWVR_GazeTriggerType::GazeTriggerType_Timeout;
	}
}

void UWaveVRInputFunctionLibrary::TriggerHapticPulse(EWVR_DeviceType device, int32 duration_ms)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(device);
	duration_ms = duration_ms > 0 ? duration_ms : 0;

	FWaveVRAPIWrapper::GetInstance()->TriggerVibration((WVR_DeviceType)_device, WVR_InputId::WVR_InputId_Alias1_Touchpad, duration_ms);
}
