// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRInputSimulator.h"
#include "WaveVRInput.h"

WaveVRInputSimulator::WaveVRInputSimulator()
	: uContext(nullptr)
	, PlayerController(nullptr)
	, viewportSize(FVector2D::ZeroVector)
	, viewportCenter(FVector2D::ZeroVector)
	, position_right(FVector::ZeroVector)
	, euler_right(FVector::ZeroVector)
	, position_left(FVector::ZeroVector)
	, euler_left(FVector::ZeroVector)
	, position_head(FVector::ZeroVector)
	, euler_head(FVector::ZeroVector)
{
}

WaveVRInputSimulator::~WaveVRInputSimulator()
{
}

void WaveVRInputSimulator::EnableSimulator(UObject * WorldContextObject)
{

	uContext = WorldContextObject;
	if (uContext != nullptr)
	{
		UWorld* wp = GEngine->GetWorldFromContextObject(uContext, EGetWorldErrorMode::LogAndReturnNull);
		if (wp != 0)
		{
			PlayerController = UGameplayStatics::GetPlayerController(wp, 0);
			if (PlayerController != nullptr)
			{
				PlayerController->bEnableClickEvents = true;
				LOGD(LogWaveVRInputSimulator, "EnableSimulator() get player controller.");
			}

			viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
			viewportCenter = FVector2D(viewportSize.X / 2, viewportSize.Y / 2);
			UE_LOG(LogWaveVRInputSimulator, Log, TEXT("EnableSimulator() viewportCenter (%f, %f)"), viewportCenter.X, viewportCenter.Y);
		}
	}
}

bool WaveVRInputSimulator::IsButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id)
{
	bool _result = false;

	if (PlayerController != nullptr)
	{
		switch (device)
		{
		case EWVR_DeviceType::DeviceType_Controller_Left:
			_result = PlayerController->IsInputKeyDown(EKeys::LeftMouseButton);
			//if (_result)
				//UE_LOG(LogWaveVRInputSimulator, Log, TEXT("IsButtonPressed() left mouse button is pressed."));
			break;
		case EWVR_DeviceType::DeviceType_Controller_Right:
			_result = PlayerController->IsInputKeyDown(EKeys::RightMouseButton);
			//if (_result)
				//UE_LOG(LogWaveVRInputSimulator, Log, TEXT("IsButtonPressed() right mouse button is pressed."));
			break;
		default:
			break;
		}
	}

	return _result;
}

/// Output pose is UE-bases coordinate.
bool WaveVRInputSimulator::GetDevicePose(FVector& OutPosition, FRotator& OutOrientation, EWVR_DeviceType type)
{
	if (PlayerController == nullptr)
		return false;

	float fX = 0, fY = 0;
	bool _ret = PlayerController->GetMousePosition(fX, fY);
	if (!_ret)
		return false;
	//UE_LOG(LogWaveVRInputSimulator, Log, TEXT("GetDevicePose() mouse:(%f, %f)"), fX, fY);

	// Right Hand
	if (PlayerController->IsInputKeyDown(EKeys::RightShift))
	{
		position_right.Z = (viewportCenter.Y - fY) / 3;
		position_right.Y = (fX - viewportCenter.X) / 5;
		//UE_LOG(LogWaveVRInputSimulator, Log, TEXT("GetDevicePose() position_right:(%f, %f, %f)"), position_right.X, position_right.Y, position_right.Z);
	}

	if (PlayerController->IsInputKeyDown(EKeys::RightAlt))
	{
		euler_right.Y = ((viewportSize.Y - fY) / viewportSize.Y) * 120;
		euler_right.Z = ((fX - viewportCenter.X) / viewportCenter.X) * 90;
		//UE_LOG(LogWaveVRInputSimulator, Log, TEXT("GetDevicePose() fY: %f, viewportSize.Y: %f, euler_right:(%f, %f, %f)"), fY, viewportSize.Y, euler_right.X, euler_right.Y, euler_right.Z);
	}
	/*
	// Left Hand
	if (PlayerController->IsInputKeyDown(EKeys::LeftShift))
	{
		position_left.Z = (viewportCenter.Y - fY) / 3;
		position_left.Y = (fX - viewportCenter.X) / 5;
	}

	if (PlayerController->IsInputKeyDown(EKeys::LeftAlt))
	{
		euler_left.Y = ((viewportSize.Y - fY) / viewportSize.Y) * 120;
		euler_left.Z = ((fX - viewportCenter.X) / viewportCenter.X) * 90;
	}
	*/
	// Head
	if (PlayerController->IsInputKeyDown(EKeys::LeftShift))
	{
		position_head.Z = (viewportCenter.Y - fY) / 3;
		position_head.Y = (fX - viewportCenter.X) / 5;
		//UE_LOG(LogWaveVRInputSimulator, Log, TEXT("GetDevicePose() position_head:(%f, %f, %f)"), position_head.X, position_head.Y, position_head.Z);
	}

	if (PlayerController->IsInputKeyDown(EKeys::LeftAlt))
	{
		euler_head.Y = ((viewportCenter.Y - fY) / viewportCenter.Y) * 90;
		euler_head.Z = ((fX - viewportCenter.X) / viewportCenter.X) * 180;
		//UE_LOG(LogWaveVRInputSimulator, Log, TEXT("GetDevicePose() euler_head:(%f, %f, %f)"), euler_head.X, euler_head.Y, euler_head.Z);
	}

	if (PlayerController->IsInputKeyDown(EKeys::T))
	{
		UWaveVRBlueprintFunctionLibrary::SetFocusController(EWVR_DeviceType::DeviceType_Controller_Right);
	}
	if (PlayerController->IsInputKeyDown(EKeys::R))
	{
		UWaveVRBlueprintFunctionLibrary::SetFocusController(EWVR_DeviceType::DeviceType_Controller_Left);
	}

	if (type == EWVR_DeviceType::DeviceType_Controller_Right)
	{
		OutPosition = position_right;
		OutOrientation = FRotator::MakeFromEuler(euler_right);
		return true;
	}
	else if (type == EWVR_DeviceType::DeviceType_Controller_Left)
	{
		OutPosition = position_left;
		OutOrientation = FRotator::MakeFromEuler(euler_left);
		return true;
	}
	else if (type == EWVR_DeviceType::DeviceType_HMD)
	{
		OutPosition = position_head;
		OutOrientation = FRotator::MakeFromEuler(euler_head);
		return true;
	}

	return false;
}
