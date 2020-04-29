// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRJson.h"
#include "WaveVRPrivatePCH.h"

//---------------------------------------------------------------------------
FString UWaveVRJson::WaveVR_GetKeyValue(FString category, FString key) {
	return FString(TEXT(""));
}

bool UWaveVRJson::WaveVR_GetKeyVector(FString category, FString key, FVector& vec) {
	return false;
}

bool UWaveVRJson::WaveVR_GetKeyVector4(FString category, FString key, FVector4& vec4) {
	return false;
}
