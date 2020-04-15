// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRPrivatePCH.h"
#include "WaveVREventCommon.h"
#include "WaveVRHMD.h"
#include "WaveVRController.h"

bool UWaveVREventCommon::bInitialized(false);

FSettingsControllerRoleChangeNative UWaveVREventCommon::OnControllerRoleChangeNative;
bool UWaveVREventCommon::bIsLeftHanded(false);
bool UWaveVREventCommon::bCheckLeftHanded(false);
bool UWaveVREventCommon::bResetButtonStates(false);

FOnResumeNative UWaveVREventCommon::OnResumeNative;

FTrackingModeChangeNative UWaveVREventCommon::OnTrackingModeChangeNative;
FOEMConfigChangeNative UWaveVREventCommon::OnOEMConfigChangeNative;
EWVR_DOF UWaveVREventCommon::currentDoF(EWVR_DOF::DOF_SYSTEM);
bool UWaveVREventCommon::bCheckTrackingMode(false);

FAllCommonEventNative UWaveVREventCommon::OnAllEventNative;

FConnectionChangeNative UWaveVREventCommon::OnConnectionChangeNative;

FTouchpadPressNative UWaveVREventCommon::OnTouchpadPressNative_Right;
FTouchpadPressNative UWaveVREventCommon::OnTouchpadPressNative_Left;
FAllButtonPressNative UWaveVREventCommon::OnAllButtonPressNative_Right;
FAllButtonPressNative UWaveVREventCommon::OnAllButtonPressNative_Left;

FAllEventPressNative UWaveVREventCommon::OnAllEventPressNative_HMD;
FAllEventPressNative UWaveVREventCommon::OnAllEventPressNative_Right;
FAllEventPressNative UWaveVREventCommon::OnAllEventPressNative_Left;

FAllEventTouchNative UWaveVREventCommon::OnAllEventTouchNative_HMD;
FAllEventTouchNative UWaveVREventCommon::OnAllEventTouchNative_Right;
FAllEventTouchNative UWaveVREventCommon::OnAllEventTouchNative_Left;

bool UWaveVREventCommon::btnPress_HMD[EventCommonButtonCount] = { false };
bool UWaveVREventCommon::btnPress_right[EventCommonButtonCount] = { false };
bool UWaveVREventCommon::btnPress_left[EventCommonButtonCount] = { false };

bool UWaveVREventCommon::btnTouch_HMD[EventCommonButtonCount] = { false };
bool UWaveVREventCommon::btnTouch_right[EventCommonButtonCount] = { false };
bool UWaveVREventCommon::btnTouch_left[EventCommonButtonCount] = { false };

FHandChanging UWaveVREventCommon::OnHandChangingNative;

#pragma region
FDynamicGestureNative UWaveVREventCommon::OnDynamicGestureNative_Right;
FDynamicGestureNative UWaveVREventCommon::OnDynamicGestureNative_Left;
#pragma endregion Gesture

// Sets default values for this component's properties
UWaveVREventCommon::UWaveVREventCommon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	isFocusCapturedBySystem = false;
}

// Called when the game starts
void UWaveVREventCommon::BeginPlay()
{
	Super::BeginPlay();

	UWaveVREventCommon::OnControllerRoleChangeNative.AddDynamic(this, &UWaveVREventCommon::OnControllerRoleChangeHandling);
	bCheckLeftHanded = true;

	UWaveVREventCommon::OnResumeNative.AddDynamic(this, &UWaveVREventCommon::OnResumeHandling);

	UWaveVREventCommon::OnTrackingModeChangeNative.AddDynamic(this, &UWaveVREventCommon::OnTrackingModeChangeHandling);
	bCheckTrackingMode = true;

	UWaveVREventCommon::OnOEMConfigChangeNative.AddDynamic(this, &UWaveVREventCommon::OnOEMConfigChangeHandling);
	UWaveVREventCommon::OnAllEventNative.AddDynamic(this, &UWaveVREventCommon::OnAllEventHandling);

	UWaveVREventCommon::OnConnectionChangeNative.AddDynamic(this, &UWaveVREventCommon::OnConnectionChangeHandling);

	UWaveVREventCommon::OnTouchpadPressNative_Right.AddDynamic(this, &UWaveVREventCommon::OnTouchpadPressHandling_Right);
	UWaveVREventCommon::OnTouchpadPressNative_Left.AddDynamic(this, &UWaveVREventCommon::OnTouchpadPressHandling_Left);
	UWaveVREventCommon::OnAllButtonPressNative_Right.AddDynamic(this, &UWaveVREventCommon::OnAllButtonPressHandling_Right);
	UWaveVREventCommon::OnAllButtonPressNative_Left.AddDynamic(this, &UWaveVREventCommon::OnAllButtonPressHandling_Left);

	UWaveVREventCommon::OnAllEventPressNative_HMD.AddDynamic(this, &UWaveVREventCommon::OnAllEventPressHandling_HMD);
	UWaveVREventCommon::OnAllEventPressNative_Right.AddDynamic(this, &UWaveVREventCommon::OnAllEventPressHandling_Right);
	UWaveVREventCommon::OnAllEventPressNative_Left.AddDynamic(this, &UWaveVREventCommon::OnAllEventPressHandling_Left);

	UWaveVREventCommon::OnAllEventTouchNative_HMD.AddDynamic(this, &UWaveVREventCommon::OnAllEventTouchHandling_HMD);
	UWaveVREventCommon::OnAllEventTouchNative_Right.AddDynamic(this, &UWaveVREventCommon::OnAllEventTouchHandling_Right);
	UWaveVREventCommon::OnAllEventTouchNative_Left.AddDynamic(this, &UWaveVREventCommon::OnAllEventTouchHandling_Left);

	UWaveVREventCommon::OnHandChangingNative.AddDynamic(this, &UWaveVREventCommon::OnHandChangingHandling);
	ChangingHand = false;

	UWaveVREventCommon::OnDynamicGestureNative_Right.AddDynamic(this, &UWaveVREventCommon::OnDynamicGestureHandling_Right);
	UWaveVREventCommon::OnDynamicGestureNative_Left.AddDynamic(this, &UWaveVREventCommon::OnDynamicGestureHandling_Left);

	bInitialized = true;
}

#pragma region
void UWaveVREventCommon::OnControllerRoleChangeHandling()
{
	bCheckLeftHanded = true;
	bResetButtonStates = true;
}

void UWaveVREventCommon::OnResumeHandling()
{
	LOGD(LogWaveVREventCommon, "OnResumeHandling()");

	OnResumeBp.Broadcast();
}

void UWaveVREventCommon::OnTrackingModeChangeHandling()
{
	LOGD(LogWaveVREventCommon, "OnTrackingModeChangeHandling() current DoF: %d", (uint8)currentDoF);

	bCheckTrackingMode = true;
}

void UWaveVREventCommon::OnOEMConfigChangeHandling()
{
	LOGD(LogWaveVREventCommon, "OnOEMConfigChangeHandling(), call to blueprint if any");

	OnOEMConfigChangeBp.Broadcast();
}

void UWaveVREventCommon::OnAllEventHandling(int32 event_num)
{
	LOGD(LogWaveVREventCommon, "OnAllEventHandling() event %d", event_num);
	OnAllEventBp.Broadcast(event_num);
}

void UWaveVREventCommon::OnHandChangingHandling(bool changing)
{
	LOGD(LogWaveVREventCommon, "OnHandChangingHandling() changing: %d", (uint8)changing);
	ChangingHand = changing;
}
#pragma endregion Handler of Native Event

void UWaveVREventCommon::OnConnectionChangeHandling(uint8 device, bool connection)
{
	LOGD(LogWaveVREventCommon, "OnConnectionChangeHandling() device: %d, connection: %d", device, (uint8)connection);
	OnConnectionChangeBp.Broadcast(device, connection);

	if (device == (uint8)EWVR_DeviceType::DeviceType_HMD)
	{
		if (connection == false)
		{
			LOGD(LogWaveVREventCommon, "OnConnectionChangeHandling() HMD is disconnected, reset HMD button states.");
			for (int i = 0; i < InputButtonCount; i++)
			{
				int _btn = (int)InputButton[i];
				btnPress_HMD[_btn] = false;
				btnTouch_HMD[_btn] = false;
			}
		}
	}

	if (device == (uint8)EWVR_DeviceType::DeviceType_Controller_Right)
	{
		if (connection == false)
		{
			LOGD(LogWaveVREventCommon, "OnConnectionChangeHandling() right controller is disconnected, reset right button states.");
			for (int i = 0; i < InputButtonCount; i++)
			{
				int _btn = (int)InputButton[i];
				btnPress_right[_btn] = false;
				btnTouch_right[_btn] = false;
			}
		}
	}

	if (device == (uint8)EWVR_DeviceType::DeviceType_Controller_Left)
	{
		if (connection == false)
		{
			LOGD(LogWaveVREventCommon, "OnConnectionChangeHandling() left controller is disconnected, reset left button states.");
			for (int i = 0; i < InputButtonCount; i++)
			{
				int _btn = (int)InputButton[i];
				btnPress_left[_btn] = false;
				btnTouch_left[_btn] = false;
			}
		}
	}
}

#pragma region
void UWaveVREventCommon::OnTouchpadPressHandling_Right(bool down)
{
	LOGD(LogWaveVREventCommon, "OnTouchpadPressHandling_Right() down: %d", (uint8)down);
	OnTouchpadPressBp_Right.Broadcast(down);
}

void UWaveVREventCommon::OnTouchpadPressHandling_Left(bool down)
{
	LOGD(LogWaveVREventCommon, "OnTouchpadPressHandling_Left() down: %d", (uint8)down);
	OnTouchpadPressBp_Left.Broadcast(down);
}

void UWaveVREventCommon::OnAllButtonPressHandling_Right(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllButtonPressHandling_Right() button: %d, down: %d", button, (uint8)down);
	OnAllButtonPressBp_Right.Broadcast(button, down);
}

void UWaveVREventCommon::OnAllButtonPressHandling_Left(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllButtonPressHandling_Left() button: %d, down: %d", button, (uint8)down);
	OnAllButtonPressBp_Left.Broadcast(button, down);
}

void UWaveVREventCommon::OnAllEventPressHandling_HMD(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllEventPressHandling_HMD() button: %d, down: %d", button, (uint8)down);
	btnPress_HMD[button] = down;
	OnAllEventPressBp_HMD.Broadcast(button, down);
}

void UWaveVREventCommon::OnAllEventPressHandling_Right(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllEventPressHandling_Right() button: %d, down: %d", button, (uint8)down);
	btnPress_right[button] = down;
	OnAllEventPressBp_Right.Broadcast(button, down);
}

void UWaveVREventCommon::OnAllEventPressHandling_Left(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllEventPressHandling_Left() button: %d, down: %d", button, (uint8)down);
	btnPress_left[button] = down;
	OnAllEventPressBp_Left.Broadcast(button, down);
}

void UWaveVREventCommon::OnAllEventTouchHandling_HMD(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllEventTouchHandling_HMD() button: %d, down: %d", button, (uint8)down);
	btnTouch_HMD[button] = down;
}

void UWaveVREventCommon::OnAllEventTouchHandling_Right(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllEventTouchHandling_Right() button: %d, down: %d", button, (uint8)down);
	btnTouch_right[button] = down;
}

void UWaveVREventCommon::OnAllEventTouchHandling_Left(uint8 button, bool down)
{
	LOGD(LogWaveVREventCommon, "OnAllEventTouchHandling_Left() button: %d, down: %d", button, (uint8)down);
	btnTouch_left[button] = down;
}
#pragma endregion Handler of Button Event Native

#pragma region
bool UWaveVREventCommon::IsControllerButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id)
{
	bool _press = false;
	if (device == EWVR_DeviceType::DeviceType_Controller_Right)
	{
		_press = isBtnPress_right(button_id);
	}
	else if (device == EWVR_DeviceType::DeviceType_Controller_Left)
	{
		_press = isBtnPress_left(button_id);
	}
	else if (device == EWVR_DeviceType::DeviceType_HMD)
	{
		_press = isBtnPress_HMD(button_id);
	}
	return _press;
}

bool UWaveVREventCommon::isBtnPress_HMD(EWVR_InputId button)
{
	return btnPress_HMD[(uint8)button];
}

bool UWaveVREventCommon::isBtnPress_right(EWVR_InputId button)
{
	return btnPress_right[(uint8)button];
}

bool UWaveVREventCommon::isBtnPress_left(EWVR_InputId button)
{
	return btnPress_left[(uint8)button];
}

bool UWaveVREventCommon::IsControllerButtonTouched(EWVR_DeviceType device, EWVR_TouchId button_id)
{
	bool _touch = false;
	if (device == EWVR_DeviceType::DeviceType_Controller_Right)
	{
		_touch = isBtnTouch_right(button_id);
	}
	else if (device == EWVR_DeviceType::DeviceType_Controller_Left)
	{
		_touch = isBtnTouch_left(button_id);
	}
	else if (device == EWVR_DeviceType::DeviceType_HMD)
	{
		_touch = isBtnTouch_HMD(button_id);
	}
	return _touch;
}

bool UWaveVREventCommon::isBtnTouch_HMD(EWVR_TouchId button)
{
	return btnTouch_HMD[(uint8)button];
}

bool UWaveVREventCommon::isBtnTouch_right(EWVR_TouchId button)
{
	return btnTouch_right[(uint8)button];
}

bool UWaveVREventCommon::isBtnTouch_left(EWVR_TouchId button)
{
	return btnTouch_left[(uint8)button];
}
#pragma endregion Get Button States

bool UWaveVREventCommon::IsLeftHandedMode()
{
	if (!bInitialized)
	{
		bIsLeftHanded = UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode();
	}
	return bIsLeftHanded;
}

EWVR_DOF UWaveVREventCommon::GetDegreeOfFreedom()
{
	if (!bInitialized)
	{
		currentDoF = UWaveVRBlueprintFunctionLibrary::IsTrackingHMDPosition() ? EWVR_DOF::DOF_6 : EWVR_DOF::DOF_3;
	}
	return currentDoF;
}

bool UWaveVREventCommon::IsHandChanging()
{
	return ChangingHand;
}


#pragma region
void UWaveVREventCommon::ShouldCheckLeftHandedOnResume(bool check)
{
	LOGD(LogWaveVREventCommon, "ShouldCheckLeftHandedOnResume() %d", check);
	bCheckLeftHanded = check;
}

void UWaveVREventCommon::ShouldResetButtonStatesOnResume(bool reset)
{
	LOGD(LogWaveVREventCommon, "ShouldResetButtonStatesOnResume() %d", reset);
	bResetButtonStates = reset;
}

void UWaveVREventCommon::ShouldCheckDoFOnResume(bool check)
{
	LOGD(LogWaveVREventCommon, "ShouldCheckDoFOnResume() %d", check);
	bCheckTrackingMode = check;
}
#pragma endregion On Application Pause Action

#pragma region
void UWaveVREventCommon::OnDynamicGestureHandling_Right(uint8 type)
{
	UE_LOG(LogWaveVREventCommon, Log, TEXT("OnDynamicGestureHandling_Right() type: %d"), type);
	OnDynamicGestureBp_Right.Broadcast(type);
}

void UWaveVREventCommon::OnDynamicGestureHandling_Left(uint8 type)
{
	UE_LOG(LogWaveVREventCommon, Log, TEXT("OnDynamicGestureHandling_Left() type: %d"), type);
	OnDynamicGestureBp_Left.Broadcast(type);
}
#pragma endregion Gesture

// Called every frame
void UWaveVREventCommon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool _focusCapturedBySystem = UWaveVRBlueprintFunctionLibrary::IsInputFocusCapturedBySystem();
	if (isFocusCapturedBySystem != _focusCapturedBySystem)
	{
		isFocusCapturedBySystem = _focusCapturedBySystem;
		if (!isFocusCapturedBySystem)
		{
			LOGD(LogWaveVREventCommon, "TickComponent() get system focus.");
			bResetButtonStates = true;
		}
	}

	if (bCheckLeftHanded)
	{
		bool hand_mode = UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode();
		if (bIsLeftHanded != hand_mode)
		{
			bIsLeftHanded = hand_mode;
			LOGD(LogWaveVREventCommon, "TickComponent() bIsLeftHanded: %d", bIsLeftHanded);
			OnControllerRoleChangeBp.Broadcast();
		}
		bCheckLeftHanded = false;
	}

	if (bResetButtonStates)
	{
		LOGD(LogWaveVREventCommon, "TickComponent() reset button states.");
		UWaveVRController::ResetButtonStates(EWVR_DeviceType::DeviceType_Controller_Right);
		UWaveVRController::ResetButtonStates(EWVR_DeviceType::DeviceType_Controller_Left);

		for (int i = 0; i < InputButtonCount; i++)
		{
			int _btn = (int)InputButton[i];
			btnPress_HMD[_btn] = UWaveVRBlueprintFunctionLibrary::GetInputButtonState(EWVR_DeviceType::DeviceType_HMD, InputButton[i]);
			btnPress_right[_btn] = UWaveVRBlueprintFunctionLibrary::GetInputButtonState(EWVR_DeviceType::DeviceType_Controller_Right, InputButton[i]);
			btnPress_left[_btn] = UWaveVRBlueprintFunctionLibrary::GetInputButtonState(EWVR_DeviceType::DeviceType_Controller_Left, InputButton[i]);
			btnTouch_HMD[_btn] = UWaveVRBlueprintFunctionLibrary::GetInputTouchState(EWVR_DeviceType::DeviceType_HMD, InputButton[i]);
			btnTouch_right[_btn] = UWaveVRBlueprintFunctionLibrary::GetInputTouchState(EWVR_DeviceType::DeviceType_Controller_Right, InputButton[i]);
			btnTouch_left[_btn] = UWaveVRBlueprintFunctionLibrary::GetInputTouchState(EWVR_DeviceType::DeviceType_Controller_Left, InputButton[i]);
			LOGD(LogWaveVREventCommon, "TickComponent() HMD button %d pressed? %d", _btn, (int)btnPress_HMD[_btn]);
			LOGD(LogWaveVREventCommon, "TickComponent() Right button %d pressed? %d", _btn, (int)btnPress_right[_btn]);
			LOGD(LogWaveVREventCommon, "TickComponent() Left button %d pressed? %d", _btn, (int)btnPress_left[_btn]);
			LOGD(LogWaveVREventCommon, "TickComponent() HMD button %d touched? %d", _btn, (int)btnTouch_HMD[_btn]);
			LOGD(LogWaveVREventCommon, "TickComponent() Right button %d touched? %d", _btn, (int)btnTouch_right[_btn]);
			LOGD(LogWaveVREventCommon, "TickComponent() Left button %d touched? %d", _btn, (int)btnTouch_left[_btn]);
		}
		bResetButtonStates = false;
	}

	if (bCheckTrackingMode)
	{
		// 0: DOF 3, 1: DOF 6, 2: DOF SYSTEM
		currentDoF = UWaveVRBlueprintFunctionLibrary::IsTrackingHMDPosition() ? EWVR_DOF::DOF_6 : EWVR_DOF::DOF_3;
		LOGD(LogWaveVREventCommon, "TickComponent() currentDoF: %d", (uint8)currentDoF);
		OnTrackingModeChangeBp.Broadcast();
		bCheckTrackingMode = false;
	}
	// ...
}

