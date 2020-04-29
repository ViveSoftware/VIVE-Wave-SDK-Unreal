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
#include "WaveVRBlueprintFunctionLibrary.h"
#include "AdaptiveControllerLoader.generated.h"

UENUM(BlueprintType)
enum class EWVR_TouchpadIndex : uint8
{
	EWVR_TouchpadIndex_Invalid = 0,
	EWVR_TouchpadIndex_Center = 1,
	EWVR_TouchpadIndex_Up = 2,
	EWVR_TouchpadIndex_Right = 3
};

USTRUCT()
struct FBatteryLevel
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int min;

	UPROPERTY()
	int max;

	UPROPERTY()
	FString path;
};

USTRUCT()
struct FBatteryInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int LevelCount;

	UPROPERTY()
	TArray<FBatteryLevel> BatteryLevel;
};

USTRUCT()
struct FBatterySetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int show;
};

DECLARE_LOG_CATEGORY_EXTERN(LogAdapCtrLoader, Log, All);

/**
 *
 */
UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API UAdaptiveControllerLoader : public UObject
{
	GENERATED_BODY()

public:
	UAdaptiveControllerLoader();
	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta=(
		DisplayName="Perform decompression task", ToolTip="This is an internal API, please do not call it directly!"))
	static bool doDeployControllerModel(EWVR_DeviceType type);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (
		DisplayName = "Get path of render model", ToolTip = "This path is ready after render model has been decompressed."))
	static FString GetRenderModelPath();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (
		DisplayName = "Get Texture2D from render model", ToolTip = "Texture2D is ready after render model has been decompressed."))
	static UTexture2D* GetTexture2DFromFile();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (
		ToolTip = "This is an internal API, please do not call it directly!"))
	static bool GetLoadingTexture2DResult();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "This is an internal API, please do not call it directly!"))
	static bool GetSpawnAdaptiveControllerFlag();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "This is an internal API, please do not call it directly!"))
	static bool GetSpawnActorResult();

	static void SetSpawnActorResult(bool ret);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "This is an internal API, please do not call it directly!"))
	static void SetSpawnAdaptiveControllerFlag(bool flag);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "This is an internal API, please do not call it directly!"))
	static bool GetUseNewModel();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "Use TouchPad index to query definition from render model."))
	static bool GetTouchPadInfo(EWVR_TouchpadIndex index, FVector& vec);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "Get defined distance to touchpad from render model"))
	static float GetTouchPadFloatingHeight();

	static void SetUseNewModel(bool ret);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (DeprecatedFunction, DeprecationMessage = "Use WaveVR|OEMConfig instead"))
	static bool IsBatteryInfo();

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "The value indicates how many battery levels defined in render model."))
	static bool GetBatteryLevels(int& levels);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|ControllerLoader", meta = (ToolTip = "Use index to query minimum, maximum values and image for this battery level."))
	static bool GetBatteryLevelInfo(int level, float& min, float& max, UTexture2D*& levelTex);

private:
	static TSharedPtr<FJsonObject> TouchJsonParsed;

	static bool GetBatteryJson();
	static bool isReadBatteryJson;
	static FString BatteryContent;
	static int mLevels;

	static bool GetTouchPadJson();
	static bool isReadTouchPadJson;
	static FString TouchPadContent;
	static FString mRenderModelPath;
	static UTexture2D* mTexture;
	static bool mSpawnControllerFlag;
	static bool mLoadingTextureRet;
	static bool mSpawnActorRet;
	static bool mUseNewModel;
};
