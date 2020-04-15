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

DEFINE_LOG_CATEGORY_STATIC(LogWaveVRInputSimulator, Log, All);
/**
 *
 */
class WaveVRInputSimulator
{
public:
	WaveVRInputSimulator();
	~WaveVRInputSimulator();

	void EnableSimulator(UObject * WorldContextObject);
	bool GetDevicePose(FVector& OutPosition, FRotator& OutOrientation, EWVR_DeviceType type = EWVR_DeviceType::DeviceType_HMD);
	bool IsButtonPressed(EWVR_DeviceType device, EWVR_InputId button_id);

private:
	UObject * uContext;
	APlayerController* PlayerController;
	FVector2D viewportSize;
	FVector2D viewportCenter;
	FVector position_right;
	FVector euler_right;
	FVector position_left;
	FVector euler_left;
	FVector position_head;
	FVector euler_head;
};
