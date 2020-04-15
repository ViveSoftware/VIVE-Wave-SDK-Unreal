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

#include <chrono>
using namespace std::chrono;

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WaveVRBlueprintFunctionLibrary.h"
#include "WaveVRController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWaveVRController, Log, All);

class Device
{
public:
	Device(EWVR_DeviceType hand);

	EWVR_DeviceType Hand;

	bool AllowPressActionInAFrame(EWVR_InputId id);
	bool GetPress(EWVR_InputId id);
	bool GetPressDown(EWVR_InputId id);
	bool GetPressUp(EWVR_InputId id);

	bool AllowTouchActionInAFrame(EWVR_TouchId id);
	bool GetTouch(EWVR_TouchId id);
	bool GetTouchDown(EWVR_TouchId id);
	bool GetTouchUp(EWVR_TouchId id);

	bool AllowGetAxisInAFrame(EWVR_TouchId id);
	FVector2D GetAxis(EWVR_TouchId id);

	void ResetButtonStates();

private:
	bool preState_press[InputButtonCount];
	bool curState_press[InputButtonCount];
	milliseconds prevFrameCount_press[InputButtonCount];

	void Update_PressState(EWVR_InputId _id);

	bool preState_touch[TouchButtonCount];
	bool curState_touch[TouchButtonCount];
	milliseconds prevFrameCount_touch[TouchButtonCount];

	void Update_TouchState(EWVR_TouchId _id);

	FVector2D curAxis;
	milliseconds prevFrameCount_axis[TouchButtonCount];
};

UCLASS(Blueprintable)
class WAVEVR_API UWaveVRController : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UWaveVRController();

	static bool IsRightControllerButtonPressed(EWVR_InputId button_id);
	static bool IsLeftControllerButtonPressed(EWVR_InputId button_id);
	static bool IsControllerButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id);
	static bool IsRightControllerButtonTouched(EWVR_TouchId button_id);
	static bool IsLeftControllerButtonTouched(EWVR_TouchId button_id);
	static bool IsControllerButtonTouched(EWVR_DeviceType device, EWVR_TouchId button_id);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Controller",
		meta = (ToolTip = "To reset all button states to false."))
	static void ResetButtonStates(EWVR_DeviceType device);

	static bool IsRightControllerConnected();
	static bool IsLeftControllerConnected();
	static bool IsControllerConnected(EWVR_DeviceType device);

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Controller",
		meta = (ToolTip = "To check if there is a button on a device pressed."))
	static bool IsAnyButtonPressed();

	static FVector2D GetRightControllerAxis(EWVR_TouchId button_id);
	static FVector2D GetLeftControllerAxis(EWVR_TouchId button_id);
	static FVector2D GetControllerAxis(EWVR_DeviceType device, EWVR_TouchId button_id);

	/*
	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressedDown(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonPressedUp(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressedDown(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonPressedUp(EWVR_InputId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouchedDown(EWVR_TouchId button_id);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsRightControllerButtonTouchedUp(EWVR_TouchId button_id);

	//UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouchedDown(EWVR_TouchId button_id);

	//UFUNCTION(BlueprintCallable, Category = "WaveVR|Controller")
	static bool IsLeftControllerButtonTouchedUp(EWVR_TouchId button_id);
	*/

	UFUNCTION(
		BlueprintCallable,
		Category = "WaveVR|Controller",
		meta = (ToolTip = "To check if there is a button on the head mount device pressed."))
	static bool IsHmdButtonPressed(EWVR_InputId button_id);

private:
	static Device HMD;
	static Device RightController;
	static Device LeftController;
};
