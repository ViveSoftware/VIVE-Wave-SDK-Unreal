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
#include "UObject/NoExportTypes.h"
#include "WaveVRJson.generated.h"

class WVRJsonImpl {
public:
	WVRJsonImpl();
	//   ~WVRJsonImpl();

	FString GetConfig(FString category, FString key);
	bool GetVector(FString category, FString key, FVector& vec);
	bool GetVector4(FString category, FString key, FVector4& vec4);
	static WVRJsonImpl* getInstance();

private:
	static WVRJsonImpl* mInstance;

#if PLATFORM_ANDROID
	FString jsonString;
	bool jsonUpdate;
	bool AndroidJavaENVCheck();
	void AndroidJavaENVFinish();
	bool mJavaAttached;
	TSharedPtr<FJsonObject> JsonParsed;
	void updateJsonStr();
#endif
};

/**
 *
 */
UCLASS()
class WAVEVR_API UWaveVRJson : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "WaveVR|JSON", meta = (DeprecatedFunction, DeprecationMessage = "Use WaveVR|OEMConfig instead"))
	static FString WaveVR_GetKeyValue(FString category, FString key);
	UFUNCTION(BlueprintCallable, Category = "WaveVR|JSON", meta = (DeprecatedFunction, DeprecationMessage = "Use WaveVR|OEMConfig instead"))
	static bool WaveVR_GetKeyVector(FString category, FString key, FVector& vec);
	UFUNCTION(BlueprintCallable, Category = "WaveVR|JSON", meta = (DeprecatedFunction, DeprecationMessage = "Use WaveVR|OEMConfig instead"))
	static bool WaveVR_GetKeyVector4(FString category, FString key, FVector4& vec4);
};
