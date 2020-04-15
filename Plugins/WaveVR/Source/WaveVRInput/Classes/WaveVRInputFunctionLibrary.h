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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WaveVRBlueprintFunctionLibrary.h"
#include "WaveVRController.h"
#include "WaveVRInputFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class EWVR_InteractionMode : uint8
{
	InteractionMode_SystemDefault,  // Application uses controller if controller is connected; otherwise switches to gaze.
	InteractionMode_Gaze,  // Gaze mode only.
	InteractionMode_Controller  // Controller mode only.
	// InteractionMode_Unknown
};

UENUM(BlueprintType)
enum class EWVR_GazeTriggerType : uint8
{
	GazeTriggerType_Timeout,  // Use timeout to trigger gaze.
	GazeTriggerType_Button,  // Use button to trigger gaze.
	GazeTriggerType_TimeoutButton  // Both timeout and button can trigger gaze.
	// GazeTriggerType_Unknown
};

/**
 *
 */
UCLASS()
class UWaveVRInputFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "DO NOT use this API due to it already has been used in WaveVRPawn to initialize WaveVRInput plugin."))
	static void InitializeWaveVRInput();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To check if the WaveVRInput feature is available."))
	static bool IsWaveVRInputInitialized();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get the orientation of the specified device."))
	static FRotator GetWaveVRInputDeviceOrientation(EWVR_DeviceType device = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get the position of the specified device."))
	static FVector GetWaveVRInputDevicePosition(EWVR_DeviceType device = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To check if the specified device is tracked."))
	static ETrackingStatus GetWaveVRInputDeviceTrackingStatus(EWVR_DeviceType device = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get the velocity of the specified device."))
	static FVector GetWaveVRInputDeviceVelocity(EWVR_DeviceType device = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get the angular velocity of the specified device."))
	static FVector GetWaveVRInputDeviceAngularVelocity(EWVR_DeviceType device = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To simulate the device position when the environment is 3DoF (rotation only)."))
	static void UseSimulationPose(SimulatePosition simulation = SimulatePosition::WhenNoPosition);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (WorldContext = WorldContextObject, ToolTip = "To simulate the device position when the environment is 3DoF (rotation only)."))
	static void EnableInputSimulator(UObject * WorldContextObject);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To make the rotation-only controller which uses the simulation pose follows the head's movement."))
	static void FollowHeadPosition(bool follow);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "This API is used for pose simulation customization."))
	static void UpdateUnitySimulationSettingsFromJson(
		FVector HEADTOELBOW_OFFSET,
		FVector ELBOWTOWRIST_OFFSET,
		FVector WRISTTOCONTROLLER_OFFSET,
		FVector ELBOW_PITCH_OFFSET,
		float ELBOW_PITCH_ANGLE_MIN,
		float ELBOW_PITCH_ANGLE_MAX
	);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To check if the specified device is available."))
	static bool IsInputAvailable(EWVR_DeviceType device = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To check if the pose of the specified device is valid."))
	static bool IsInputPoseValid(EWVR_DeviceType device = EWVR_DeviceType::DeviceType_Controller_Right);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To check if the specified button of the specified device is pressed."))
	static bool IsInputButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To check if the specified button of the specified device is touched."))
	static bool IsInputButtonTouched(EWVR_DeviceType device, EWVR_TouchId button_id);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get the specified button's axis."))
	static FVector2D GetInputButtonAxis(EWVR_DeviceType device, EWVR_TouchId button_id);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To set the interaction mode: Gaze or Controller."))
	static bool SetInteractionMode(EWVR_InteractionMode mode);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get the current interaction mode."))
	static EWVR_InteractionMode GetInteractionMode();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To set the gaze trigger type: Timer or Button or both."))
	static void SetGazeTriggerType(EWVR_GazeTriggerType type);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get the current gaze trigger type."))
	static EWVR_GazeTriggerType GetGazeTriggerType();

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To vibrate the specified device in a duration of microseconds."))
	static void TriggerHapticPulse(EWVR_DeviceType device, int32 duration_ms);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To know the button mapping information. E.g. Right thumbstick button is mapping to dominant touchpad key event."))
	static EWVR_InputId GetInputMappingPair(EWVR_DeviceType type, EWVR_InputId button);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To get connected device's battery percentage."))
	static float GetDeviceBatteryPercentage(EWVR_DeviceType type);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Input",
		meta = (ToolTip = "To check if the button of the specified device is available."))
	static bool IsButtonAvailable(EWVR_DeviceType type, EWVR_InputId button);
};
