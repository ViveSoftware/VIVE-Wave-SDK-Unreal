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
#include "WaveVRResourceWrapper.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogResourceWrapper, Log, All);

class ResourceWrapperImpl {
public:
	ResourceWrapperImpl();
 //   ~ResourceWrapperImpl();

	FString getString(FString stringName);

	FString getStringByLanguage(FString stringName, FString lang, FString country);

	FString getSystemLanguage();

	FString getSystemCountry();

	bool setPreferredLanguage(FString lang, FString country);

	void useSystemLanguage();

	static ResourceWrapperImpl* getInstance();

private:
	static ResourceWrapperImpl* mInstance;
	FString mPreferredLanguage;
	FString mCountry;
	bool useSystemLanguageFlag = true;
};

/**
 *
 */
UCLASS()
class WAVEVR_API UWaveVRResourceWrapper : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Resource", meta = (
		ToolTip = "Return system defined value by key you provided."))
	static FString WaveVR_GetString(FString stringName);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Resource", meta = (
		ToolTip = "Return value by key and language you preferred."))
	static FString WaveVR_GetStringByLanguage(FString stringName, FString lang, FString country);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Resource", meta = (
		ToolTip = "Return language code what your device is."))
	static FString WaveVR_GetSystemLanguage();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Resource", meta = (
		ToolTip = "Return country code where your device is located in."))
	static FString WaveVR_GetSystemCountry();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Resource", meta = (
		ToolTip = "Updates preferred language and country"))
	static bool WaveVR_SetPreferredLanguage(FString lang, FString country);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Resource", meta = (
		ToolTip = "Return string will be defined by system locale after this action."))
	static void WaveVR_UseSystemLanguage();
};
