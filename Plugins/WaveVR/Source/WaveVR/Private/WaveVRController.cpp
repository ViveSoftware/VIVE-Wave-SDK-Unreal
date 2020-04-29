// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRController.h"
#include "WaveVRPrivatePCH.h"
#include "Platforms/Editor/WaveVRDirectPreview.h"
#include "PoseManagerImp.h"
#include "WaveVRHMD.h"


DEFINE_LOG_CATEGORY(LogWaveVRController);

#pragma region Timer
bool Device::AllowPressActionInAFrame(EWVR_InputId id)
{
	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			milliseconds _ms = duration_cast< milliseconds >(
				system_clock::now().time_since_epoch()
				);
			if (_ms != prevFrameCount_press[i])
			{
				prevFrameCount_press[i] = _ms;
				return true;
			}
		}
	}

	return false;
}

bool Device::AllowTouchActionInAFrame(EWVR_TouchId id)
{
	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			milliseconds _ms = duration_cast< milliseconds >(
				system_clock::now().time_since_epoch()
				);
			if (_ms != prevFrameCount_touch[i])
			{
				prevFrameCount_touch[i] = _ms;
				return true;
			}
		}
	}

	return false;
}

bool Device::AllowGetAxisInAFrame(EWVR_TouchId id)
{
	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			milliseconds _ms = duration_cast< milliseconds >(
				system_clock::now().time_since_epoch()
				);
			if (_ms != prevFrameCount_axis[i])
			{
				prevFrameCount_axis[i] = _ms;
				return true;
			}
		}
	}

	return false;
}
#pragma endregion

#pragma region Class Device
Device::Device(EWVR_DeviceType hand)
{
	Hand = hand;
	ResetButtonStates();
	curAxis = FVector2D::ZeroVector;
}

void Device::ResetButtonStates()
{
	for (int i = 0; i < InputButtonCount; i++)
	{
		preState_press[i] = false;
		curState_press[i] = false;
		prevFrameCount_press[i] = { milliseconds(0) };
	}
	for (int i = 0; i < TouchButtonCount; i++)
	{
		preState_touch[i] = false;
		curState_touch[i] = false;
		prevFrameCount_touch[i] = { milliseconds(0) };
		prevFrameCount_axis[i] = { milliseconds(0) };
	}
}

// -------------------- Press --------------------
void Device::Update_PressState(EWVR_InputId id)
{
	if (!AllowPressActionInAFrame(id))
		return;

	bool _pressed = false;

	if (WaveVRDirectPreview::IsDirectPreview() || !GIsEditor)
	{
		FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
		if (HMD != nullptr && HMD->IsDeviceConnected((WVR_DeviceType)Hand))
		{
			_pressed = FWaveVRAPIWrapper::GetInstance()->GetInputButtonState((WVR_DeviceType)Hand, (WVR_InputId)id);
		}
	}

	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			preState_press[i] = curState_press[i];
			curState_press[i] = _pressed;
			break;
		}
	}
}

bool Device::GetPress(EWVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			_state = curState_press[i];
			break;
		}
	}

	return _state;
}

bool Device::GetPressDown(EWVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			if (curState_press[i] == true && preState_press[i] == false)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

bool Device::GetPressUp(EWVR_InputId id)
{
	bool _state = false;
	Update_PressState(id);

	for (unsigned int i = 0; i < InputButtonCount; i++)
	{
		if (id == InputButton[i])
		{
			if (curState_press[i] == false && preState_press[i] == true)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

// -------------------- Touch --------------------
void Device::Update_TouchState(EWVR_TouchId id)
{
	bool _isTouchBtn = false;
	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			_isTouchBtn = true;
			break;
		}
	}
	if (!_isTouchBtn)
	{
		LOGD(LogWaveVRController, "Update_TouchState() invalid id %d", (uint8)id);
		return;
	}
	if (!AllowTouchActionInAFrame(id))
		return;

	bool _touched = false;
	if (WaveVRDirectPreview::IsDirectPreview() || !GIsEditor)
	{
		FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
		if (HMD != nullptr && HMD->IsDeviceConnected((WVR_DeviceType)Hand))
		{
			_touched = FWaveVRAPIWrapper::GetInstance()->GetInputTouchState((WVR_DeviceType)Hand, (WVR_InputId)id);
		}
	}

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			preState_touch[i] = curState_touch[i];
			curState_touch[i] = _touched;
			break;
		}
	}
}

bool Device::GetTouch(EWVR_TouchId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			_state = curState_touch[i];
			break;
		}
	}

	return _state;
}

bool Device::GetTouchDown(EWVR_TouchId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			if (curState_touch[i] == true && preState_touch[i] == false)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

bool Device::GetTouchUp(EWVR_TouchId id)
{
	bool _state = false;
	Update_TouchState(id);

	for (unsigned int i = 0; i < TouchButtonCount; i++)
	{
		if (id == TouchButton[i])
		{
			if (curState_touch[i] == false && preState_touch[i] == true)
			{
				_state = true;
			}
			break;
		}
	}

	return _state;
}

FVector2D Device::GetAxis(EWVR_TouchId id)
{
	bool _allow = false;
	WVR_Axis_t _axis = {0, 0};
	FVector2D _vec = FVector2D::ZeroVector;

	if (AllowGetAxisInAFrame(id))
	{
		if (WaveVRDirectPreview::IsDirectPreview() || !GIsEditor) {
			_axis = FWaveVRAPIWrapper::GetInstance()->GetInputAnalogAxis((WVR_DeviceType)Hand, (WVR_InputId)id);
			//LOGD(LogWaveVRController, "GetAxis(), id %d axis:{%f, %f}", (uint8)id, _axis.x, _axis.y);
		}
		_allow = true;
	}

	switch (id)
	{
	case EWVR_TouchId::Touchpad:
		if (_allow)
		{
			curAxis.X = _axis.x;
			curAxis.Y = _axis.y;
		}
		// If updating axis is now allowed, use previous axis.
		_vec = curAxis;
		break;
	case EWVR_TouchId::Trigger:
		if (_allow)
		{
			curAxis.X = _axis.x;
		}
		// If updating axis is now allowed, use previous axis.
		_vec = curAxis;
		break;
	default:
		break;
	}

	return _vec;
}
#pragma endregion

#pragma region Class UWaveVRController
UWaveVRController::UWaveVRController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

Device UWaveVRController::HMD(EWVR_DeviceType::DeviceType_HMD);
Device UWaveVRController::RightController(EWVR_DeviceType::DeviceType_Controller_Right);
Device UWaveVRController::LeftController(EWVR_DeviceType::DeviceType_Controller_Left);

bool UWaveVRController::IsRightControllerButtonPressed(EWVR_InputId button_id)
{
	bool _pressed = RightController.GetPress(button_id);

	return _pressed;
}

bool UWaveVRController::IsLeftControllerButtonPressed(EWVR_InputId button_id)
{
	bool _pressed = LeftController.GetPress(button_id);

	return _pressed;
}

bool UWaveVRController::IsHmdButtonPressed(EWVR_InputId button_id)
{
	bool _pressed = HMD.GetPress(button_id);
	return _pressed;
}

bool UWaveVRController::IsControllerButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id)
{
	bool _result = false;

	switch (device)
	{
	case EWVR_DeviceType::DeviceType_Controller_Right:
		_result = IsRightControllerButtonPressed(button_id);
		break;
	case EWVR_DeviceType::DeviceType_Controller_Left:
		_result = IsLeftControllerButtonPressed(button_id);
		break;
	default:
		break;
	}

	return _result;
}

bool UWaveVRController::IsRightControllerButtonTouched(EWVR_TouchId button_id)
{
	bool _touched = RightController.GetTouch(button_id);

	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouched(EWVR_TouchId button_id)
{
	bool _touched = LeftController.GetTouch(button_id);

	return _touched;
}

bool UWaveVRController::IsControllerButtonTouched(EWVR_DeviceType device, EWVR_TouchId button_id)
{
	bool _result = false;

	switch (device)
	{
	case EWVR_DeviceType::DeviceType_Controller_Right:
		_result = IsRightControllerButtonTouched(button_id);
		break;
	case EWVR_DeviceType::DeviceType_Controller_Left:
		_result = IsLeftControllerButtonTouched(button_id);
		break;
	default:
		break;
	}

	return _result;
}

void UWaveVRController::ResetButtonStates(EWVR_DeviceType device)
{
	switch (device)
	{
	case EWVR_DeviceType::DeviceType_Controller_Right:
		RightController.ResetButtonStates();
		break;
	case EWVR_DeviceType::DeviceType_Controller_Left:
		LeftController.ResetButtonStates();
		break;
	default:
		break;
	}
}

bool UWaveVRController::IsRightControllerConnected()
{
	bool _connected = false;
		bool IsDirectPreview = WaveVRDirectPreview::IsDirectPreview();

#if WITH_EDITOR
	if (GIsEditor && !IsDirectPreview)
		return true;
#endif // WITH_EDITOR

	if (WaveVRDirectPreview::IsDirectPreview() || !GIsEditor) {
		FWaveVRHMD* WVRHMD = FWaveVRHMD::GetInstance();
		if (WVRHMD != nullptr && WVRHMD->IsDeviceConnected(WVR_DeviceType_Controller_Right))
			_connected = true;
	}

	return _connected;
}

bool UWaveVRController::IsLeftControllerConnected()
{
	bool _connected = false;
		bool IsDirectPreview = WaveVRDirectPreview::IsDirectPreview();

#if WITH_EDITOR
	if (GIsEditor && !IsDirectPreview)
		return true;
#endif // WITH_EDITOR

	if (WaveVRDirectPreview::IsDirectPreview() || !GIsEditor) {
		FWaveVRHMD* WVRHMD = FWaveVRHMD::GetInstance();
		if (WVRHMD != nullptr && WVRHMD->IsDeviceConnected(WVR_DeviceType_Controller_Left))
			_connected = true;
	}

	return _connected;
}

bool UWaveVRController::IsControllerConnected(EWVR_DeviceType device)
{
	bool _connected = false;
	bool IsDirectPreview = WaveVRDirectPreview::IsDirectPreview();

#if WITH_EDITOR
	if (GIsEditor && !IsDirectPreview)
		return true;
#endif // WITH_EDITOR

	if (WaveVRDirectPreview::IsDirectPreview() || !GIsEditor)
	{
		FWaveVRHMD* WVRHMD = FWaveVRHMD::GetInstance();
		if (WVRHMD == nullptr)
			return false;

		switch (device)
		{
		case EWVR_DeviceType::DeviceType_Controller_Right:
			_connected = WVRHMD->IsDeviceConnected(WVR_DeviceType_Controller_Right);
			break;
		case EWVR_DeviceType::DeviceType_Controller_Left:
			_connected = WVRHMD->IsDeviceConnected(WVR_DeviceType_Controller_Left);
			break;
		default:
			break;
		}
	}

	return _connected;
}

bool UWaveVRController::IsAnyButtonPressed()
{
	bool _pressed = false;

	if (IsRightControllerConnected())
	{
		for (unsigned int i = 0; i < InputButtonCount; i++) {
			EWVR_InputId button_id = InputButton[i];
			_pressed = IsRightControllerButtonPressed(button_id);
			if (_pressed)
			{
				LOGD(LogWaveVRController, "IsAnyButtonPressed() right %d is pressed", (uint8)button_id);
				break;
			}
		}
	}
	else if (IsLeftControllerConnected())
	{
		for (unsigned int i = 0; i < (sizeof(InputButton) / sizeof(*InputButton)); i++) {
			EWVR_InputId button_id = InputButton[i];
			_pressed = IsLeftControllerButtonPressed(button_id);
			if (_pressed)
			{
				LOGD(LogWaveVRController, "IsAnyButtonPressed() left %d is pressed", (uint8)button_id);
				break;
			}
		}
	}

	return _pressed;
}

FVector2D UWaveVRController::GetRightControllerAxis(EWVR_TouchId button_id)
{
	FVector2D _vec = RightController.GetAxis(button_id);

	return _vec;
}

FVector2D UWaveVRController::GetLeftControllerAxis(EWVR_TouchId button_id)
{
	FVector2D _vec = LeftController.GetAxis(button_id);

	return _vec;
}

FVector2D UWaveVRController::GetControllerAxis(EWVR_DeviceType device, EWVR_TouchId button_id)
{
	FVector2D _vec = FVector2D::ZeroVector;

	switch (device)
	{
	case EWVR_DeviceType::DeviceType_Controller_Right:
		_vec = GetRightControllerAxis(button_id);
		break;
	case EWVR_DeviceType::DeviceType_Controller_Left:
		_vec = GetLeftControllerAxis(button_id);
		break;
	default:
		break;
	}

	return _vec;
}

/*
bool UWaveVRController::IsRightControllerButtonPressedDown(EWVR_InputId button_id)
{
	bool _state = RightController.GetPressDown(button_id);

	return _state;
}

bool UWaveVRController::IsRightControllerButtonPressedUp(EWVR_InputId button_id)
{
	bool _state = RightController.GetPressUp(button_id);

	return _state;
}

bool UWaveVRController::IsLeftControllerButtonPressedDown(EWVR_InputId button_id)
{
	bool _state = LeftController.GetPressDown(button_id);

	return _state;
}

bool UWaveVRController::IsLeftControllerButtonPressedUp(EWVR_InputId button_id)
{
	bool _state = LeftController.GetPressUp(button_id);

	return _state;
}

bool UWaveVRController::IsRightControllerButtonTouchedDown(EWVR_TouchId button_id)
{
	bool _touched = RightController.GetTouchDown(button_id);

	return _touched;
}

bool UWaveVRController::IsRightControllerButtonTouchedUp(EWVR_TouchId button_id)
{
	bool _touched = RightController.GetTouchUp(button_id);

	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouchedDown(EWVR_TouchId button_id)
{
	bool _touched = LeftController.GetTouchDown(button_id);

	return _touched;
}

bool UWaveVRController::IsLeftControllerButtonTouchedUp(EWVR_TouchId button_id)
{
	bool _touched = LeftController.GetTouchUp(button_id);

	return _touched;
}
*/
#pragma endregion
