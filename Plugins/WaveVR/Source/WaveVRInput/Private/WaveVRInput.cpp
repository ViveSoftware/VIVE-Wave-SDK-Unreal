// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRInput.h"
#include "PoseManagerImp.h"
#include "Features/IModularFeatures.h"
#include "WaveVRBlueprintFunctionLibrary.h"
#include "Platforms/Editor/WaveVRDirectPreview.h"
#include "WaveVREventCommon.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

#include "WaveVRUtils.h"

using namespace wvr::utils;

#pragma region
static bool IsPlayInEditor()
{
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.World()->IsPlayInEditor())
		{
			return true;
		}
	}
	return false;
}

float QuaternionLengthSquared(FQuat q)
{
	return
		q.W * q.W +
		q.X * q.X +
		q.Y * q.Y +
		q.Z * q.Z;
}

FVector QuaternionRotate(FQuat& q, const FVector v)
{
	const float w = q.W;
	const float x = q.X;
	const float y = q.Y;
	const float z = q.Z;

	const float kTwo = 2.0f;
	float vcoeff = kTwo * w * w - 1.0f;
	float ucoeff = kTwo * (x * v.X + y * v.Y + z * v.Z);
	float ccoeff = kTwo * w;

	float vx = vcoeff * v.X + ucoeff * x + ccoeff * (y * v.Z - z * v.Y);
	float vy = vcoeff * v.Y + ucoeff * y + ccoeff * (z * v.X - x * v.Z);
	float vz = vcoeff * v.Z + ucoeff * z + ccoeff * (x * v.Y - y * v.X);

	return FVector(vx, vy, vz);
}

float QuaternionDot(FQuat q1, FQuat q2)
{
	return q1.W * q2.W + q1.X * q2.X + q1.Y * q2.Y + q1.Z * q2.Z;
}

float QuaternionAngle(FQuat q1, FQuat q2)
{
	float f = QuaternionDot(q1, q2);
	return FMath::Acos(FMath::Min(FMath::Abs(f), 1.0f)) * 2.0f * 57.29578f;
}

FQuat FromToRotation(const FVector& from_direction, const FVector& to_direction)
{
	float dot = FVector::DotProduct(from_direction, to_direction);
	float squareFrom = FMath::Square(from_direction.X) + FMath::Square(from_direction.Y) + FMath::Square(from_direction.Z);
	float squareTo = FMath::Square(to_direction.X) + FMath::Square(to_direction.Y) + FMath::Square(to_direction.Z);
	float norm = sqrt(squareFrom * squareTo);
	float real = norm + dot;

	FVector w = FVector::ZeroVector;
	if (real < 1.e-6f * norm)
	{
		real = 0.0f;
		w = fabsf(from_direction.X) > fabsf(from_direction.Z) ?
			FVector(-from_direction.Y, from_direction.X, 0.0f) : FVector(0.0f, -from_direction.Z, from_direction.Y);
	}
	else
	{
		w = FVector(
			from_direction.Y * to_direction.Z - from_direction.Z * to_direction.Y,
			from_direction.Z * to_direction.X - from_direction.X * to_direction.Z,
			from_direction.X * to_direction.Y - from_direction.Y * to_direction.X);
	}

	FQuat result = FQuat(w.X, w.Y, w.Z, real);
	result.Normalize();

	return result;
}

FVector VectorSlerp(FVector start, FVector end, float filter) {
	// Make sure both start and end are normalized.
	start.Normalize();
	end.Normalize();
	float dot = FVector::DotProduct(start, end);
	dot = FMath::Clamp(dot, -1.0f, 1.0f);
	float theta = FMath::Acos(dot) * filter;
	FVector relative_vector = end - start * dot;
	relative_vector.Normalize();
	return ((start * FMath::Cos(theta)) + (relative_vector * FMath::Sin(theta)));
}

FQuat QuaternionSlerp(FQuat a, FQuat b, float t)
{
	t = FMath::Clamp<float>(t, 0, 1);

	if (QuaternionLengthSquared(a) == 0 && QuaternionLengthSquared(b) == 0)
		return FQuat::Identity;
	if (QuaternionLengthSquared(a) == 0)
		return b;
	if (QuaternionLengthSquared(b) == 0)
		return a;

	FVector va = FVector(a.X, a.Y, a.Z);
	FVector vb = FVector(b.X, b.Y, b.Z);
	float cosHalfAngle = a.W * b.W + FVector::DotProduct(va, vb);

	if (cosHalfAngle >= 1.0f || cosHalfAngle <= -1.0f)
	{
		// angle = 0.0f, so just return one input.
		return a;
	}
	else if (cosHalfAngle < 0.0f)
	{
		vb = -vb;
		b.W = -b.W;
		cosHalfAngle = -cosHalfAngle;
	}

	float blendA;
	float blendB;
	if (cosHalfAngle < 0.99f)
	{
		// do proper slerp for big angles
		float halfAngle = FMath::Acos(cosHalfAngle);
		float sinHalfAngle = FMath::Sin(halfAngle);
		float oneOverSinHalfAngle = 1.0f / sinHalfAngle;
		blendA = FMath::Sin(halfAngle * (1.0f - t)) * oneOverSinHalfAngle;
		blendB = FMath::Sin(halfAngle * t) * oneOverSinHalfAngle;
	}
	else
	{
		// do lerp if angle is really small.
		blendA = 1.0f - t;
		blendB = t;
	}

	FQuat result = FQuat(blendA * va + blendB * vb, blendA * a.W + blendB * b.W);
	if (QuaternionLengthSquared(result) > 0.0f)
	{
		result.Normalize();
		return result;
	}
	else
	{
		return FQuat::Identity;
	}
}

FQuat QuaternionLerp(FQuat a, FQuat b, float t)
{
	FQuat q = FQuat::Identity;
	t = FMath::Clamp<float>(t, 0, 1);

	float last = 1.0f - t;
	float dot = QuaternionDot(a, b);
	if (dot >= 0)
	{
		q.X = (last * a.X) + (t * b.X);
		q.Y = (last * a.Y) + (t * b.Y);
		q.Z = (last * a.Z) + (t * b.Z);
		q.W = (last * a.W) + (t * b.W);
	}
	else
	{
		q.X = (last * a.X) - (t * b.X);
		q.Y = (last * a.Y) - (t * b.Y);
		q.Z = (last * a.Z) - (t * b.Z);
		q.W = (last * a.W) - (t * b.W);
	}
	float squared = QuaternionLengthSquared(q);
	float deno = 1 / FMath::Sqrt(squared);
	q.X *= deno;
	q.Y *= deno;
	q.Z *= deno;
	q.W *= deno;
	return q;
}
#pragma endregion non-class function


FName FWaveVRInput::DeviceTypeName(TEXT("WaveVRInput"));

FWaveVRInput::FWaveVRInput(WaveVRInputSimulator* simulator, const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler )
	: pSimulator(simulator)
	, bInputInitialized(false)
	, fFPS(0)
	, enumUseSimulationPose(SimulatePosition::WhenNoPosition)
	, framesOfFreeze(0)
	, FollowHead(false)
	, defaultHeadUEPosition(FVector::ZeroVector)
	, defaultHeadUnityPosition(FVector::ZeroVector)
	, bodyDirection(FVector::ZeroVector)
	, bodyRotation(FQuat::Identity)
	, simulateUnityPosition(FVector::ZeroVector)
	, simulateUEPosition(FVector::ZeroVector)
	, simulateUnityQuaternion(FQuat::Identity)
	, simulateUERotation(FRotator::ZeroRotator)
	, v3ChangeArmYAxis(FVector(1, 1, 1))
	, bIsLeftHanded(false)
	, MessageHandler(InMessageHandler)
{
	IModularFeatures::Get().RegisterModularFeature( GetModularFeatureName(), this );

	for (int i = 0; i < ControllerCount; i++)
	{
		for (int j = 0; j < InputButtonCount; j++)
			PrevPressState[i][j] = false;
		for (int j = 0; j < TouchButtonCount; j++)
			PrevTouchState[i][j] = false;
	}

	int32 right_hand = (int32)EControllerHand::Right;
	int32 left_hand = (int32)EControllerHand::Left;

	// ----------------- Press Buttons --------------------
	// EWVR_InputId::Menu
	ControllerPressButtons[right_hand][0] = FGamepadKeyNames::MotionController_Right_FaceButton1;
	ControllerPressButtons[left_hand][0] = FGamepadKeyNames::MotionController_Left_FaceButton1;
	// EWVR_InputId::Grip
	ControllerPressButtons[right_hand][1] = FGamepadKeyNames::MotionController_Right_Grip1;
	ControllerPressButtons[left_hand][1] = FGamepadKeyNames::MotionController_Left_Grip1;
	// EWVR_InputId::DPad_Left
	ControllerPressButtons[right_hand][2] = FGamepadKeyNames::MotionController_Right_Thumbstick_Left;
	ControllerPressButtons[left_hand][2] = FGamepadKeyNames::MotionController_Left_Thumbstick_Left;
	// EWVR_InputId::DPad_Up
	ControllerPressButtons[right_hand][3] = FGamepadKeyNames::MotionController_Right_Thumbstick_Up;
	ControllerPressButtons[left_hand][3] = FGamepadKeyNames::MotionController_Left_Thumbstick_Up;
	// EWVR_InputId::DPad_Right
	ControllerPressButtons[right_hand][4] = FGamepadKeyNames::MotionController_Right_Thumbstick_Right;
	ControllerPressButtons[left_hand][4] = FGamepadKeyNames::MotionController_Left_Thumbstick_Right;
	// EWVR_InputId::DPad_Down
	ControllerPressButtons[right_hand][5] = FGamepadKeyNames::MotionController_Right_Thumbstick_Down;
	ControllerPressButtons[left_hand][5] = FGamepadKeyNames::MotionController_Left_Thumbstick_Down;
	// EWVR_InputId::Volume_Up
	ControllerPressButtons[right_hand][6] = WaveVRControllerKeyNames::AndroidVolumeUp;
	ControllerPressButtons[left_hand][6] = WaveVRControllerKeyNames::AndroidVolumeUp;
	// EWVR_InputId::Volume_Down
	ControllerPressButtons[right_hand][7] = WaveVRControllerKeyNames::AndroidVolumeDown;
	ControllerPressButtons[left_hand][7] = WaveVRControllerKeyNames::AndroidVolumeDown;
	// EWVR_InputId::HMD_Back
	if (!EKeys::IsGamepadKey(WaveVRControllerKeyNames::HmdBackPress))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key HmdBackPress"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::HmdBackPress, LOCTEXT("WVRHmdBackPress", "HMD Back Press"), FKeyDetails::GamepadKey));
	}
	ControllerPressButtons[right_hand][8] = WaveVRControllerKeyNames::HmdBackPress.GetFName();
	ControllerPressButtons[left_hand][8] = WaveVRControllerKeyNames::HmdBackPress.GetFName();
	// EWVR_InputId::HMD_Enter
	if (!EKeys::IsGamepadKey(WaveVRControllerKeyNames::HmdEnterPress))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key HmdEnterPress"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::HmdEnterPress, LOCTEXT("WVRHmdEnterPress", "HMD Enter Press"), FKeyDetails::GamepadKey));
	}
	ControllerPressButtons[right_hand][9] = WaveVRControllerKeyNames::HmdEnterPress.GetFName();
	ControllerPressButtons[left_hand][9] = WaveVRControllerKeyNames::HmdEnterPress.GetFName();
	// EWVR_InputId::Touchpad
	ControllerPressButtons[right_hand][10] = FGamepadKeyNames::MotionController_Right_Thumbstick;
	ControllerPressButtons[left_hand][10] = FGamepadKeyNames::MotionController_Left_Thumbstick;
	// EWVR_InputId::Trigger
	ControllerPressButtons[right_hand][11] = FGamepadKeyNames::MotionController_Right_Trigger;
	ControllerPressButtons[left_hand][11] = FGamepadKeyNames::MotionController_Left_Trigger;
	// EWVR_InputId::Thumbstick
	ControllerPressButtons[right_hand][12] = FGamepadKeyNames::MotionController_Right_Thumbstick;
	ControllerPressButtons[left_hand][12] = FGamepadKeyNames::MotionController_Left_Thumbstick;
	/* EWVR_TouchId::Joystick
	if (!EKeys::IsGamepadKey(WaveVRControllerKeyNames::LeftJoystickPress))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key LeftJoystickPress"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::LeftJoystickPress, LOCTEXT("WVRLeftJoystickPress", "Left Joystick Press"), FKeyDetails::GamepadKey));
	}
	if (!EKeys::IsGamepadKey(WaveVRControllerKeyNames::RightJoystickPress))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key RightJoystickPress"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::RightJoystickPress, LOCTEXT("WVRRightJoystickPress", "Right Joystick Press"), FKeyDetails::GamepadKey));
	}
	ControllerPressButtons[right_hand][11] = WaveVRControllerKeyNames::RightJoystickPress.GetFName();
	ControllerPressButtons[left_hand][11] = WaveVRControllerKeyNames::LeftJoystickPress.GetFName();
	*/

	// -------------- Touch Buttons ----------------
	// EWVR_TouchId::Touchpad
	ControllerTouchButtons[right_hand][0] = WaveVRControllerKeyNames::RightTouchpadTouch;
	ControllerTouchButtons[left_hand][0] = WaveVRControllerKeyNames::LeftTouchpadTouch;
	// EWVR_TouchId::Trigger
	ControllerTouchButtons[right_hand][1] = WaveVRControllerKeyNames::RightTriggerTouch;
	ControllerTouchButtons[left_hand][1] = WaveVRControllerKeyNames::LeftTriggerTouch;
	// EWVR_TouchId::Thumbstick
	ControllerTouchButtons[right_hand][2] = WaveVRControllerKeyNames::RightTouchpadTouch;
	ControllerTouchButtons[left_hand][2] = WaveVRControllerKeyNames::LeftTouchpadTouch;
	/* EWVR_TouchId::Joystick
	if (!EKeys::IsGamepadKey(WaveVRControllerKeyNames::LeftJoystickTouch))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key LeftJoystickTouch"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::LeftJoystickTouch, LOCTEXT("WVRLeftJoystickTouch", "Left Joystick Touch"), FKeyDetails::GamepadKey));
	}
	if (!EKeys::IsAxis(WaveVRControllerKeyNames::LeftJoystickAxisX))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key LeftJoystickAxisX"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::LeftJoystickAxisX, LOCTEXT("WVRLeftJoystickAxisX", "Left Joystick Axis-X"), FKeyDetails::FloatAxis));
	}
	if (!EKeys::IsAxis(WaveVRControllerKeyNames::LeftJoystickAxisY))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key LeftJoystickAxisY"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::LeftJoystickAxisY, LOCTEXT("WVRLeftJoystickAxisY", "Left Joystick Axis-Y"), FKeyDetails::FloatAxis));
	}
	if (!EKeys::IsGamepadKey(WaveVRControllerKeyNames::RightJoystickTouch))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key RightJoystickTouch"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::RightJoystickTouch, LOCTEXT("WVRRightJoystickTouch", "Right Joystick Touch"), FKeyDetails::GamepadKey));
	}
	if (!EKeys::IsAxis(WaveVRControllerKeyNames::RightJoystickAxisX))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key RightJoystickAxisX"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::RightJoystickAxisX, LOCTEXT("WVRRightJoystickAxisX", "Right Joystick Axis-X"), FKeyDetails::FloatAxis));
	}
	if (!EKeys::IsAxis(WaveVRControllerKeyNames::RightJoystickAxisY))
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("Add key RightJoystickAxisY"));
		EKeys::AddKey(FKeyDetails(WaveVRControllerKeyNames::RightJoystickAxisY, LOCTEXT("WVRRightJoystickAxisY", "Right Joystick Axis-Y"), FKeyDetails::FloatAxis));
	}

	ControllerTouchButtons[right_hand][2] = WaveVRControllerKeyNames::RightJoystickTouch.GetFName();
	ControllerTouchButtons[left_hand][2] = WaveVRControllerKeyNames::LeftJoystickTouch.GetFName();
	*/

	for (unsigned int i = 0; i < EWVR_DeviceType_Count; i++)
	{
		uePose[i].localRotation = FRotator::ZeroRotator;
		uePose[i].localPosition = FVector::ZeroVector;
		uePose_pev[i].localRotation = FRotator::ZeroRotator;
		uePose_pev[i].localPosition = FVector::ZeroVector;
		unityPose[i].pos = FVector::ZeroVector;
		unityPose[i].rot = FQuat::Identity;
		unityPose_prev[i].pos = FVector::ZeroVector;
		unityPose_prev[i].rot = FQuat::Identity;
		bPoseIsValid[i] = false;
		CurrentDoF[i] = EWVR_DOF::DOF_3;
	}

	bIsLeftHanded = UWaveVRBlueprintFunctionLibrary::IsLeftHandedMode();

	LOGD(LogWaveVRInput, "WaveVRInput is initialized");
	bInputInitialized = true;
}


FWaveVRInput::~FWaveVRInput()
{
	IModularFeatures::Get().UnregisterModularFeature( GetModularFeatureName(), this );
}

bool FWaveVRInput::IsInputInitialized()
{
	return bInputInitialized;
}

void FWaveVRInput::fireButtonPressEvent(FName button_name, bool down)
{
	if (button_name == FGamepadKeyNames::MotionController_Right_Thumbstick ||
		button_name == FGamepadKeyNames::MotionController_Left_Thumbstick ||
		button_name == FGamepadKeyNames::MotionController_Right_Trigger ||
		button_name == FGamepadKeyNames::MotionController_Left_Trigger)
	{
		LOGD(LogWaveVRInput, "fireButtonPressEvent() button %s is press %s.", TCHAR_TO_ANSI(*button_name.ToString()), (down ? "down" : "up"));
	}
	// ---- Touchpad ----
	if (button_name == FGamepadKeyNames::MotionController_Right_Thumbstick)
	{
		UWaveVREventCommon::OnTouchpadPressNative_Right.Broadcast(down);
	}
	if (button_name == FGamepadKeyNames::MotionController_Left_Thumbstick)
	{
		UWaveVREventCommon::OnTouchpadPressNative_Left.Broadcast(down);
	}
}

void FWaveVRInput::fireAllButtonPressEvent(EControllerHand hand, EWVR_InputId id, bool down)
{
	LOGD(LogWaveVRInput, "fireAllButtonPressEvent() hand %d button %d is press %s.", (uint8)hand, (uint8)id, (down ? "down" : "up"));
	switch (hand)
	{
	case EControllerHand::Right:
		UWaveVREventCommon::OnAllButtonPressNative_Right.Broadcast((uint8)id, down);
		break;
	case EControllerHand::Left:
		UWaveVREventCommon::OnAllButtonPressNative_Left.Broadcast((uint8)id, down);
		break;
	default:
		break;
	}
}

void FWaveVRInput::UpdateButtonPressStates(EControllerHand hand)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(hand);

	int32 _hand = (int32)hand;
	bool _curPressState[InputButtonCount] = { false };

	// Get current buttons' states.
	for (int i = 0; i < InputButtonCount; i++)
	{
		if (IsPlayInEditor() && !WaveVRDirectPreview::IsDirectPreview())
		{
			_curPressState[i] = pSimulator->IsButtonPressed(_device, InputButton[i]);
		}
		else
		{
			//_curPressState[i] = UWaveVRController::IsControllerButtonPressed(_device, InputButton[i]);
			_curPressState[i] = UWaveVREventCommon::IsControllerButtonPressed(_device, InputButton[i]);
		}
	}

	// Compare with previous buttons' states.
	for (int i = 0; i < InputButtonCount; i++)
	{
		FName button_name = ControllerPressButtons[_hand][i];

		if (_curPressState[i] != PrevPressState[_hand][i])
		{
			if (_curPressState[i])
			{
				LOGD(LogWaveVRInput, "UpdateButtonPressStates() hand %d device %d button %s is pressed.", _hand, (int32)_device, TCHAR_TO_ANSI(*button_name.ToString()));
				fireButtonPressEvent(button_name, true);
				fireAllButtonPressEvent(hand, InputButton[i], true);
				MessageHandler->OnControllerButtonPressed(button_name, 0, false);
			}
			else
			{
				LOGD(LogWaveVRInput, "UpdateButtonPressStates() hand %d device %d button %s is released.", _hand, (int32)_device, TCHAR_TO_ANSI(*button_name.ToString()));
				fireButtonPressEvent(button_name, false);
				fireAllButtonPressEvent(hand, InputButton[i], false);
				MessageHandler->OnControllerButtonReleased(button_name, 0, false);
			}
		}

		PrevPressState[_hand][i] = _curPressState[i];
	}
}

void FWaveVRInput::UpdateButtonTouchStates(EControllerHand hand)
{
	EWVR_DeviceType _device = GetLeftHandedDevice(hand);

	int32 _hand = (int32)hand;
	bool _curTouchState[TouchButtonCount] = { false };

	for (int i = 0; i < TouchButtonCount; i++)
	{
		//_curTouchState[i] = UWaveVRController::IsControllerButtonTouched(_device, TouchButton[i]);
		if (!IsPlayInEditor() || WaveVRDirectPreview::IsDirectPreview())
		{
			_curTouchState[i] = UWaveVREventCommon::IsControllerButtonTouched(_device, TouchButton[i]);
		}

		if (_curTouchState[i])
		{
			FVector2D axis = UWaveVRController::GetControllerAxis(_device, TouchButton[i]);
			//LOGD(LogWaveVRInput, "UpdateButtonTouchStates() hand %d device %d axis (%f, %f)", _hand, (int32)_device, axis.X, axis.Y);
			float trigger_axis = 0;
			switch (TouchButton[i])
			{
			case EWVR_TouchId::Touchpad:
			case EWVR_TouchId::Thumbstick:
				if (hand == EControllerHand::Right)
				{
					MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_Thumbstick_X, 0, axis.X);
					MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_Thumbstick_Y, 0, axis.Y);
				}
				else if (hand == EControllerHand::Left)
				{
					MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Left_Thumbstick_X, 0, axis.X);
					MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Left_Thumbstick_Y, 0, axis.Y);
				}
				break;
			case EWVR_TouchId::Trigger:
				trigger_axis = axis.X < 0 ? -(axis.X) : axis.X;
				if (hand == EControllerHand::Right)
				{
					MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_TriggerAxis, 0, trigger_axis);
				}
				else if (hand == EControllerHand::Left)
				{
					MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Left_TriggerAxis, 0, trigger_axis);
				}
				break;
				/*
			case EWVR_TouchId::Joystick:
				if (hand == EControllerHand::Right)
				{
					MessageHandler->OnControllerAnalog(WaveVRControllerKeyNames::RightJoystickAxisX.GetFName(), 0, axis.X);
					MessageHandler->OnControllerAnalog(WaveVRControllerKeyNames::RightJoystickAxisY.GetFName(), 0, axis.Y);
				}
				else if (hand == EControllerHand::Left)
				{
					MessageHandler->OnControllerAnalog(WaveVRControllerKeyNames::LeftJoystickAxisX.GetFName(), 0, axis.X);
					MessageHandler->OnControllerAnalog(WaveVRControllerKeyNames::LeftJoystickAxisY.GetFName(), 0, axis.Y);
				}
				break;
				*/
			default:
				break;
			}
		}
		else
		{
			MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_Thumbstick_X, 0, 0);
			MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_Thumbstick_Y, 0, 0);
			MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Left_Thumbstick_X, 0, 0);
			MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Left_Thumbstick_Y, 0, 0);
			MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Right_TriggerAxis, 0, 0);
			MessageHandler->OnControllerAnalog(FGamepadKeyNames::MotionController_Left_TriggerAxis, 0, 0);
		}
	}

	for (int i = 0; i < TouchButtonCount; i++)
	{
		FName button_name = ControllerTouchButtons[_hand][i];

		if (_curTouchState[i] != PrevTouchState[_hand][i])
		{
			if (_curTouchState[i])
			{
				LOGD(LogWaveVRInput, "UpdateButtonTouchStates() hand %d device %d button %s is touched.", _hand, (int32)_device, TCHAR_TO_ANSI(*button_name.ToString()));
				MessageHandler->OnControllerButtonPressed(button_name, 0, false);
			}
			else
			{
				LOGD(LogWaveVRInput, "UpdateButtonTouchStates() hand %d device %d button %s is untouched.", _hand, (int32)_device, TCHAR_TO_ANSI(*button_name.ToString()));
				MessageHandler->OnControllerButtonReleased(button_name, 0, false);
			}
		}

		PrevTouchState[_hand][i] = _curTouchState[i];
	}
}

void FWaveVRInput::EnableInputSimulator(UObject * WorldContextObject)
{
	LOGD(LogWaveVRInput, "EnableInputSimulator");
	pSimulator->EnableSimulator(WorldContextObject);
}

#pragma region
void FWaveVRInput::SendControllerEvents()
{
	//LOGD(LogWaveVRInput, "SendControllerEvents()");

	UpdateButtonPressStates(EControllerHand::Right);
	UpdateButtonPressStates(EControllerHand::Left);
	UpdateButtonTouchStates(EControllerHand::Right);
	UpdateButtonTouchStates(EControllerHand::Left);
}
#pragma endregion IInputDevice overrides

#pragma region
bool FWaveVRInput::GetControllerOrientationAndPosition( const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const
{
	bool bIsValid = false;

	unsigned int _typeIndex = (unsigned int)EWVR_DeviceType::DeviceType_Controller_Right;    // default right
	if (DeviceHand == EControllerHand::Left)
	{
		_typeIndex = (unsigned int)EWVR_DeviceType::DeviceType_Controller_Left;
	}
	else if (DeviceHand == EControllerHand::ExternalCamera)
	{
		_typeIndex = (unsigned int)EWVR_DeviceType::DeviceType_HMD;
	}
	bIsValid = bPoseIsValid[_typeIndex];
	OutPosition = uePose[_typeIndex].localPosition;
	OutOrientation = uePose[_typeIndex].localRotation;

	return bIsValid;
}

ETrackingStatus FWaveVRInput::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
	EWVR_DeviceType _device = EWVR_DeviceType::DeviceType_HMD;
	switch (DeviceHand)
	{
	case EControllerHand::Right:
		_device = EWVR_DeviceType::DeviceType_Controller_Right;
		break;
	case EControllerHand::Left:
		_device = EWVR_DeviceType::DeviceType_Controller_Left;
		break;
	default:
		break;
	}

	ETrackingStatus status = ETrackingStatus::NotTracked;

	switch (DeviceHand)
	{
	case EControllerHand::Right:
	case EControllerHand::Left:
		if (UWaveVRController::IsControllerConnected(_device))
		{
			status = ETrackingStatus::InertialOnly;

			FVector _position = FVector::ZeroVector;
			FRotator _rotation = FRotator::ZeroRotator;
			bool _valid = UWaveVRBlueprintFunctionLibrary::GetDevicePose(_position, _rotation, _device);
			if (_valid)
			{
				status = ETrackingStatus::Tracked;
			}
		}
		break;
	case EControllerHand::AnyHand:
		if (UWaveVRController::IsRightControllerConnected() || UWaveVRController::IsLeftControllerConnected())
		{
			status = ETrackingStatus::InertialOnly;
		}
		break;
	default:
		break;
	}

	return status;
}
#pragma endregion IMotionController overrides

void FWaveVRInput::SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values)
{
}

void FWaveVRInput::GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const
{
	MinFrequency = 0.0f;
	MaxFrequency = 1.0f;
}

float FWaveVRInput::GetHapticAmplitudeScale() const
{
	return 1.0f;
}

void FWaveVRInput::Tick(float DeltaTime)
{
	fFPS = 1 / DeltaTime;
	//UE_LOG(LogWaveVRInput, Log, TEXT("Tick() fps is %f, simulation: %d"), fFPS, enumUseSimulationPose);
	bIsLeftHanded = UWaveVREventCommon::IsLeftHandedMode();
	UpdatePose();
}

#pragma region
void FWaveVRInput::UpdatePose()
{
	FWaveVRHMD* HMD = GetWaveVRHMD();
	if (HMD == nullptr) return;

	// Get pose and transform to Unity coordinate.
	for (unsigned int i = 1; i < EWVR_DeviceType_Count; i++)    // 0 is DeviceType_Invalid
	{
		uePose_pev[i].localPosition = uePose[i].localPosition;
		uePose_pev[i].localRotation = uePose[i].localRotation;
		unityPose_prev[i].pos = unityPose[i].pos;
		unityPose_prev[i].rot = unityPose[i].rot;
		EWVR_DeviceType _type = (EWVR_DeviceType)i;
		CurrentDoF[i] = UWaveVRBlueprintFunctionLibrary::GetSupportedNumOfDoF(_type);

		if (IsPlayInEditor() && !WaveVRDirectPreview::IsDirectPreview())
		{
			bPoseIsValid[i] = pSimulator->GetDevicePose(uePose[i].localPosition, uePose[i].localRotation, _type);
		}
		else
		{
			bPoseIsValid[i] = UWaveVRBlueprintFunctionLibrary::GetDevicePose(uePose[i].localPosition, uePose[i].localRotation, _type);
		}
		if (bPoseIsValid[i])
		{
			unityPose[i].pos = CoordinateUtil::ConvertToUnityVector(uePose[i].localPosition, 1 / HMD->GetWorldToMetersScale());
			unityPose[i].rot = CoordinateUtil::ConvertToUnityQuaternion(uePose[i].localRotation);
		}
	}

	// Update final pose in Unreal coordinate.
	for (unsigned int i = 1; i < EWVR_DeviceType_Count; i++)    // 0 is DeviceType_Invalid
	{
		if (bPoseIsValid[i])
		{
			EWVR_DeviceType _type = (EWVR_DeviceType)i;
			if (_type == EWVR_DeviceType::DeviceType_HMD)
			{
				uePose[i].localPosition = CoordinateUtil::ConvertToUnrealVector(unityPose[i].pos, HMD->GetWorldToMetersScale());
				uePose[i].localRotation = CoordinateUtil::ConvertToUnrealRotator(unityPose[i].rot);
			}
			else
			{
				if (IsPlayInEditor())
				{
					bool _simulate_pose = false;

					if (enumUseSimulationPose == SimulatePosition::ForceSimulation ||
						(enumUseSimulationPose == SimulatePosition::WhenNoPosition &&
							CurrentDoF[i] == EWVR_DOF::DOF_3 || CurrentDoF[i] == EWVR_DOF::DOF_SYSTEM))
					{
						_simulate_pose = true;
					}

					if (_simulate_pose)
					{
						switch (_type)
						{
						case EWVR_DeviceType::DeviceType_Controller_Right:
							v3ChangeArmYAxis.X = 1;
							break;
						case EWVR_DeviceType::DeviceType_Controller_Left:
							v3ChangeArmYAxis.X = -1;
							break;
						default:
							break;
						}

						UpdateControllerPose(_type);

						simulateUEPosition = CoordinateUtil::ConvertToUnrealVector(simulateUnityPosition, HMD->GetWorldToMetersScale());
						simulateUERotation = CoordinateUtil::ConvertToUnrealRotator(simulateUnityQuaternion);

						uePose[i].localPosition = simulateUEPosition;
						uePose[i].localRotation = simulateUERotation;
					}
				}
			}
		}
		else
		{
			uePose[i].localPosition = uePose_pev[i].localPosition;
			uePose[i].localRotation = uePose_pev[i].localRotation;
		}
	}
}

void FWaveVRInput::UpdateControllerPose(EWVR_DeviceType hand)
{
	if (hand != EWVR_DeviceType::DeviceType_Controller_Left && hand != EWVR_DeviceType::DeviceType_Controller_Right)
		return;
	bodyRotation = FQuat::Identity;
	UpdateHeadAndBodyPose(hand);
	ComputeControllerUnityPose(unityPose[(unsigned int)hand]);
}

void FWaveVRInput::UpdateHeadAndBodyPose(EWVR_DeviceType hand)
{
	FVector gazeDirection = GetHeadUnityForward();
	gazeDirection.Y = 0.0f;
	gazeDirection.Normalize();

	float _bodyLerpFilter = BodyRotationFilter(unityPose_prev[(unsigned int)hand], unityPose[(unsigned int)hand]);
	if (_bodyLerpFilter > 0 && !FollowHead)
	{
		defaultHeadUnityPosition = unityPose[(unsigned int)EWVR_DeviceType::DeviceType_HMD].pos;
	}

	//bodyDirection = FMath::Lerp(bodyDirection, gazeDirection, _bodyLerpFilter);
	//bodyDirection = bodyDirection * (1.0f - _bodyLerpFilter) + gazeDirection * _bodyLerpFilter;
	//bodyDirection = UKismetMathLibrary::VLerp(bodyDirection, gazeDirection, _bodyLerpFilter);
	bodyDirection = VectorSlerp(bodyDirection, gazeDirection, _bodyLerpFilter);
	//bodyRotation = UKismetMathLibrary::FindLookAtRotation(UNITY_FORWARD, bodyDirection).Quaternion();
	bodyRotation = FromToRotation(UNITY_FORWARD, bodyDirection);
	if (WAVEVR_DEBUG)
	{
		LOGD(LogWaveVRInput, "UpdateHeadAndBodyPose() gazeDirection (%f, %f, %f)", gazeDirection.X, gazeDirection.Y, gazeDirection.Z);
		LOGD(LogWaveVRInput, "UpdateHeadAndBodyPose() bodyRotation (%f, %f, %f, %f)", bodyRotation.W, bodyRotation.X, bodyRotation.Y, bodyRotation.Z);
	}
}

float FWaveVRInput::BodyRotationFilter(RigidTransform pre_pose, RigidTransform pose)
{
	float _bodyLerpFilter = 0;

	FQuat _rot_old = pre_pose.rot;
	FQuat _rot_new = pose.rot;
	float _rot_XY_angle_old = 0, _rot_XY_angle_new = 0;

	FVector _rot_forward = FVector::ZeroVector;
	FQuat _rot_XY_rotation = FQuat::Identity;

	_rot_forward = _rot_old * UNITY_FORWARD;
	//_rot_XY_rotation = UKismetMathLibrary::FindLookAtRotation(UNITY_FORWARD, _rot_forward).Quaternion();
	_rot_XY_rotation = FromToRotation(UNITY_FORWARD, _rot_forward);
	_rot_XY_angle_old = QuaternionAngle(_rot_XY_rotation, FQuat::Identity);

	_rot_forward = _rot_new * UNITY_FORWARD;
	//_rot_XY_rotation = UKismetMathLibrary::FindLookAtRotation(UNITY_FORWARD, _rot_forward).Quaternion();
	_rot_XY_rotation = FromToRotation(UNITY_FORWARD, _rot_forward);
	_rot_XY_angle_new = QuaternionAngle(_rot_XY_rotation, FQuat::Identity);

	float _diff_angle = _rot_XY_angle_new - _rot_XY_angle_old;
	_diff_angle = _diff_angle > 0 ? _diff_angle : -_diff_angle;

	_bodyLerpFilter = FMath::Clamp<float>((_diff_angle - BodyAngleBound) / BodyAngleLimitation, 0, 1.0f);
	framesOfFreeze = _bodyLerpFilter < 1.0f ? framesOfFreeze + 1 : 0;
	if (WAVEVR_DEBUG)
	{
		LOGD(LogWaveVRInput, "BodyRotationFilter() _bodyLerpFilter %f, framesOfFreeze %d", _bodyLerpFilter, framesOfFreeze);
	}

	if (framesOfFreeze <= fFPS)
		return _bodyLerpFilter;
	else
		return 0;
}

/// <summary>
/// Get the simulated position of controller.
///
/// Consider the parts construct controller position:
/// Parts contain elbow, wrist and controller and each part has default offset from head.
/// 1. simulated elbow offset = default elbow offset apply body rotation = body rotation (Quaternion) * elbow offset (Vector3)
/// 2. simulated wrist offset = default wrist offset apply elbow rotation = elbow rotation (Quaternion) * wrist offset (Vector3)
/// 3. simulated controller offset = default controller offset apply wrist rotation = wrist rotation (Quat) * controller offset (V3)
/// head + 1 + 2 + 3 = controller position.
/// </summary>
/// <param name="rtPose">RigidTransform</param>
void FWaveVRInput::ComputeControllerUnityPose(RigidTransform rtPose)
{
	// if bodyRotation angle is θ, _inverseBodyRation is -θ
	// the operator * of Quaternion in Unity means concatenation, not multipler.
	// If quaternion qA has angle θ, quaternion qB has angle ε,
	// qA * qB will plus θ and ε which means rotating angle θ then rotating angle ε.
	// (_inverseBodyRotation * rotation of controller in world space) means angle ε subtracts angle θ.
	FQuat _controllerRotation = bodyRotation.Inverse() * rtPose.rot;
	FVector _headPosition = GetHeadUnityPosition();

	/// 1. simulated elbow offset = default elbow offset apply body rotation = body rotation (Quaternion) * elbow offset (Vector3)
	// Default left / right elbow offset.
	FVector _elbowOffset = UNITY_HEADTOELBOW_OFFSET * v3ChangeArmYAxis;
	// Default left / right elbow pitch offset.
	FVector _elbowPitchOffset = UNITY_ELBOW_PITCH_OFFSET * v3ChangeArmYAxis;

	// Use controller pitch to simulate elbow pitch.
	// Range from UNITY_ELBOW_PITCH_ANGLE_MIN ~ UNITY_ELBOW_PITCH_ANGLE_MAX.
	// The percent of pitch angle will be used to calculate the position offset.
	FVector _controllerForward = _controllerRotation * UNITY_FORWARD;
	float _controllerPitch = 90.0f - FMath::RadiansToDegrees(acosf(FVector::DotProduct(_controllerForward, UNITY_UP)));
	float _controllerPitchRadio = (_controllerPitch - UNITY_ELBOW_PITCH_ANGLE_MIN) / (UNITY_ELBOW_PITCH_ANGLE_MAX - UNITY_ELBOW_PITCH_ANGLE_MIN);
	_controllerPitchRadio = FMath::Clamp<float>(_controllerPitchRadio, 0.0f, 1.0f);

	// According to pitch angle percent, plus offset to elbow position.
	_elbowOffset += _elbowPitchOffset * _controllerPitchRadio;
	// Apply body rotation and head position to calculate final elbow position.
	_elbowOffset = _headPosition + bodyRotation * _elbowOffset;

	FVector _controllerRight = _controllerRotation * UNITY_RIGHT;
	/// 2. simulated wrist offset = default wrist offset apply elbow rotation = elbow rotation (Quaternion) * wrist offset (Vector3)
	// Rotation from Z-axis to XY-plane used to simulated elbow & wrist rotation.
	//FQuat _controllerXYRotation = UKismetMathLibrary::FindLookAtRotation(UNITY_FORWARD, _controllerForward).Quaternion();
	FQuat _controllerXYRotation = FromToRotation(UNITY_FORWARD, _controllerForward);
	float _xy_angle = QuaternionAngle(_controllerXYRotation, FQuat::Identity);
	float _controllerXYRotationRadio = _xy_angle / 180;
	// Simulate the elbow raising curve.
	float _elbowCurveLerpValue = ELBOW_TO_XYPLANE_LERP_MIN + (_controllerXYRotationRadio * (ELBOW_TO_XYPLANE_LERP_MAX - ELBOW_TO_XYPLANE_LERP_MIN));
	//FQuat _controllerXYLerpRotation = FQuat::Identity * (1.0f - _elbowCurveLerpValue) + _controllerXYRotation * _elbowCurveLerpValue;
	FQuat _controllerXYLerpRotation = QuaternionLerp(FQuat::Identity, _controllerXYRotation, _elbowCurveLerpValue);

	// Default left / right wrist offset
	FVector _wristOffset = UNITY_ELBOWTOWRIST_OFFSET * v3ChangeArmYAxis;
	// elbow rotation + curve = wrist rotation
	// wrist rotation = controller XY rotation
	// => elbow rotation + curve = controller XY rotation
	// => elbow rotation = controller XY rotation - curve
	FQuat _elbowRotation = bodyRotation * _controllerXYLerpRotation.Inverse() * _controllerXYRotation;
	// Apply elbow offset and elbow rotation to calculate final wrist position.
	_wristOffset = _elbowOffset + _elbowRotation * _wristOffset;


	/// 3. simulated controller offset = default controller offset apply wrist rotation = wrist rotation (Quat) * controller offset (V3)
	// Default left / right controller offset.
	FVector _controllerOffset = UNITY_WRISTTOCONTROLLER_OFFSET * v3ChangeArmYAxis;
	FQuat _wristRotation = _controllerXYRotation;
	// Apply wrist offset and wrist rotation to calculate final controller position.
	_controllerOffset = _wristOffset + _wristRotation * _controllerOffset;

	if (WAVEVR_DEBUG)
	{
		LOGD(LogWaveVRInput, "ComputeControllerPose() _elbowPitchOffset (%f, %f, %f)", _elbowPitchOffset.X, _elbowPitchOffset.Y, _elbowPitchOffset.Z);
		LOGD(LogWaveVRInput, "ComputeControllerPose() _controllerPitch: %f, _controllerPitchRadio: %f", _controllerPitch, _controllerPitchRadio);
		LOGD(LogWaveVRInput, "ComputeControllerPose() _elbowOffset (%f, %f, %f)", _elbowOffset.X, _elbowOffset.Y, _elbowOffset.Z);
		LOGD(LogWaveVRInput, "ComputeControllerPose() _wristOffset (%f, %f, %f)", _wristOffset.X, _wristOffset.Y, _wristOffset.Z);
		LOGD(LogWaveVRInput, "ComputeControllerPose() _controllerOffset (%f, %f, %f)", _controllerOffset.X, _controllerOffset.Y, _controllerOffset.Z);
	}

	simulateUnityPosition = /*bodyRotation */ _controllerOffset;
	simulateUnityQuaternion = bodyRotation * _controllerRotation;
}

FVector FWaveVRInput::GetHeadUEPosition()
{
	FVector _headpos = uePose[(unsigned int)EWVR_DeviceType::DeviceType_HMD].localPosition;
	return FollowHead ? _headpos : defaultHeadUEPosition;
}

FVector FWaveVRInput::GetHeadUnityPosition()
{
	FVector _headpos = unityPose[(unsigned int)EWVR_DeviceType::DeviceType_HMD].pos;
	return FollowHead ? _headpos : defaultHeadUnityPosition;
}

FQuat FWaveVRInput::GetHeadUnityRotation()
{
	FQuat _headrot = FQuat::Identity;
	if (bPoseIsValid[(unsigned int)EWVR_DeviceType::DeviceType_HMD])
	{
		_headrot = unityPose[(unsigned int)EWVR_DeviceType::DeviceType_HMD].rot;
	}
	return _headrot;
}

FVector FWaveVRInput::GetHeadUnityForward()
{
	FQuat _head_rot = GetHeadUnityRotation();
	FVector _forward = _head_rot * UNITY_FORWARD;
	return _forward;
}

bool FWaveVRInput::IsLeftHandedMode()
{
	return bIsLeftHanded;
}

EWVR_DeviceType FWaveVRInput::GetLeftHandedDevice(EControllerHand hand)
{
	EWVR_DeviceType _device = EWVR_DeviceType::DeviceType_HMD;
	switch (hand)
	{
	case EControllerHand::Right:
		_device = bIsLeftHanded ? EWVR_DeviceType::DeviceType_Controller_Left : EWVR_DeviceType::DeviceType_Controller_Right;
		break;
	case EControllerHand::Left:
		_device = bIsLeftHanded ? EWVR_DeviceType::DeviceType_Controller_Right : EWVR_DeviceType::DeviceType_Controller_Left;
		break;
	default:
		break;
	}

	return _device;
}

void FWaveVRInput::UseSimulationPose(SimulatePosition simulation)
{
	LOGD(LogWaveVRInput, "UseSimulationPose() simulation: %d.", (uint8)simulation);
	enumUseSimulationPose = simulation;
	UWaveVRBlueprintFunctionLibrary::SetPoseSimulationOption(enumUseSimulationPose);
}

void FWaveVRInput::FollowHeadPosition(bool follow)
{
	FollowHead = follow;
	UWaveVRBlueprintFunctionLibrary::SetFollowHead(follow);
}

void FWaveVRInput::UpdateUnitySimulationSettingsFromJson(
	FVector HEADTOELBOW_OFFSET,
	FVector ELBOWTOWRIST_OFFSET,
	FVector WRISTTOCONTROLLER_OFFSET,
	FVector ELBOW_PITCH_OFFSET,
	float ELBOW_PITCH_ANGLE_MIN,
	float ELBOW_PITCH_ANGLE_MAX)
{
	LOGD(LogWaveVRInput, "UpdateUnitySimulationSettingsFromJson() Before update:");
	LOGD(LogWaveVRInput, "UNITY_HEADTOELBOW_OFFSET (%f, %f, %f)", UNITY_HEADTOELBOW_OFFSET.X, UNITY_HEADTOELBOW_OFFSET.Y, UNITY_HEADTOELBOW_OFFSET.Z);
	LOGD(LogWaveVRInput, "UNITY_ELBOWTOWRIST_OFFSET (%f, %f, %f)", UNITY_ELBOWTOWRIST_OFFSET.X, UNITY_ELBOWTOWRIST_OFFSET.Y, UNITY_ELBOWTOWRIST_OFFSET.Z);
	LOGD(LogWaveVRInput, "WRISTTOCONTROLLER_OFFSET (%f, %f, %f)", WRISTTOCONTROLLER_OFFSET.X, WRISTTOCONTROLLER_OFFSET.Y, WRISTTOCONTROLLER_OFFSET.Z);
	LOGD(LogWaveVRInput, "ELBOW_PITCH_OFFSET (%f, %f, %f)", ELBOW_PITCH_OFFSET.X, ELBOW_PITCH_OFFSET.Y, ELBOW_PITCH_OFFSET.Z);
	LOGD(LogWaveVRInput, "ELBOW_PITCH_ANGLE_MIN (%f), ELBOW_PITCH_ANGLE_MAX (%f)", ELBOW_PITCH_ANGLE_MIN, ELBOW_PITCH_ANGLE_MAX);

	UNITY_HEADTOELBOW_OFFSET = HEADTOELBOW_OFFSET;
	UNITY_ELBOWTOWRIST_OFFSET = ELBOWTOWRIST_OFFSET;
	UNITY_WRISTTOCONTROLLER_OFFSET = WRISTTOCONTROLLER_OFFSET;
	UNITY_ELBOW_PITCH_OFFSET = ELBOW_PITCH_OFFSET;
	UNITY_ELBOW_PITCH_ANGLE_MIN = ELBOW_PITCH_ANGLE_MIN;
	UNITY_ELBOW_PITCH_ANGLE_MAX = ELBOW_PITCH_ANGLE_MAX;

	LOGD(LogWaveVRInput, "UpdateUnitySimulationSettingsFromJson() After update:");
	LOGD(LogWaveVRInput, "UNITY_HEADTOELBOW_OFFSET (%f, %f, %f)", UNITY_HEADTOELBOW_OFFSET.X, UNITY_HEADTOELBOW_OFFSET.Y, UNITY_HEADTOELBOW_OFFSET.Z);
	LOGD(LogWaveVRInput, "UNITY_ELBOWTOWRIST_OFFSET (%f, %f, %f)", UNITY_ELBOWTOWRIST_OFFSET.X, UNITY_ELBOWTOWRIST_OFFSET.Y, UNITY_ELBOWTOWRIST_OFFSET.Z);
	LOGD(LogWaveVRInput, "WRISTTOCONTROLLER_OFFSET (%f, %f, %f)", WRISTTOCONTROLLER_OFFSET.X, WRISTTOCONTROLLER_OFFSET.Y, WRISTTOCONTROLLER_OFFSET.Z);
	LOGD(LogWaveVRInput, "ELBOW_PITCH_OFFSET (%f, %f, %f)", ELBOW_PITCH_OFFSET.X, ELBOW_PITCH_OFFSET.Y, ELBOW_PITCH_OFFSET.Z);
	LOGD(LogWaveVRInput, "ELBOW_PITCH_ANGLE_MIN (%f), ELBOW_PITCH_ANGLE_MAX (%f)", ELBOW_PITCH_ANGLE_MIN, ELBOW_PITCH_ANGLE_MAX);
}
#pragma endregion Controller Position Simulation
