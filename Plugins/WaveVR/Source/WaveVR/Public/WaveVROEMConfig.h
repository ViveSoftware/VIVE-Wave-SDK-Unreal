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
#include "WaveVROEMConfig.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOEMConfig, Log, All);

USTRUCT()
struct FOEnableSingleBeam
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString enable;
};

USTRUCT()
struct FOBatterySetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int show;
};

class WaveVROEMConfigImpl {
public:
	WaveVROEMConfigImpl();
	//   ~WVRJsonImpl();

	FString GetConfig(FString category, FString key);
	bool GetVector(FString category, FString key, FVector& vec);
	bool GetVector4(FString category, FString key, FVector4& vec4);
	bool IsEnableSingleBeam();
	bool IsBatteryInfo();
	static WaveVROEMConfigImpl* getInstance();

private:
	static WaveVROEMConfigImpl* mInstance;

	FString jsonString;
	bool jsonUpdate;
	TSharedPtr<FJsonObject> JsonParsed;
	void updateJsonStr();
};

/**
 *
 */
UCLASS()
class WAVEVR_API UWaveVROEMConfig : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "WaveVR|OEMConfig", meta = (
		ToolTip = "Return true if SingleBeam feature is enable by OEM Config."))
	static bool WaveVR_IsEnableSingleBeam();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|OEMConfig", meta = (
		ToolTip = "Return true if Battery Info will show by OEM Config. However, it also depends on render model definition."))
	static bool WaveVR_IsBatteryInfo();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|OEMConfig", meta = (
		ToolTip = "Using category and key as index to retrieve string value from OEM Config."))
	static FString WaveVR_GetKeyValue(FString category, FString key);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|OEMConfig", meta = (
		ToolTip = "Using category and key as index to retrieve Vector value from OEM Config."))
	static bool WaveVR_GetKeyVector(FString category, FString key, FVector& vec);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|OEMConfig", meta = (
		ToolTip = "Using category and key as index to retrieve Vector4 value from OEM Config."))
	static bool WaveVR_GetKeyVector4(FString category, FString key, FVector4& vec4);
};
