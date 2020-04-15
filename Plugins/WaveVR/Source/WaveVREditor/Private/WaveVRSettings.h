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

#include "WaveVRSettings.generated.h"

UENUM()
enum class EFoveatedRenderingPeripheralQuality : uint8
{
	Low = 0x0,		/* power saving is high. */
	Medium = 0x1,	/* power saving is medium. */
	High = 0x2,		/* power saving is low. */
};

UENUM()
enum class EDirectPreviewConnectType : uint8
{
	USB,
	WiFi
};

UENUM()
enum class EDirectPreviewUpdateFrequency : uint8
{
	RuntimeDefined,
	FPS_15,
	FPS_30,
	FPS_45,
	FPS_60,
	FPS_75
};

/**
 * WaveVR developer settings
 */
UCLASS(config=Engine, defaultconfig)
class WAVEVREDITOR_API UWaveVRSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, config, Category = Render, meta = (
		ConsoleVariable = "wvr.EyeTracking.enable", DisplayName = "Enable Eye tracking",
		ToolTip = "If not supported by device, it will be disabled."))
	bool bEyeTrackingEnable;

	UPROPERTY(EditAnywhere, config, Category = Render, meta = (
		ConsoleVariable = "wvr.RenderMask.enable", DisplayName = "Enable Render Mask",
		ToolTip = "RenderMask help to skip the rendering to the area of eye buffer which is not seen by player.  If device is not supported, it will be disable.  And it will be also disabled if HMD's LateUpdate is enabled.  However the LateUpdate is default enabled now."))
	bool bRenderMaskEnable;

	UPROPERTY(EditAnywhere, config, Category = Render, meta = (
		ConsoleVariable = "wvr.FoveatedRendering.enable", DisplayName = "Enable Foveated Rendering",
		ToolTip = "Foveated Rendering helps save device power."))
	bool bFoveatedRenderingEnable;

	UPROPERTY(EditAnywhere, config, Category = Render, meta = (
		ConsoleVariable = "wvr.FoveatedRendering.peripheralFOV", DisplayName = "Foveated Rendering Peripheral FOV",
		ToolTip = "Where to start to decrease quality.", ClampMin = "1", ClampMax = "179"))
	uint32 FoveatedRenderingPeripheralFOV;

	UPROPERTY(EditAnywhere, config, Category = Render, meta = (
		ConsoleVariable = "wvr.FoveatedRendering.peripheralQuality", DisplayName = "Foveated Rendering Peripheral Quality",
		ToolTip = "Quality High means the image quality is good but power saving is low."))
	EFoveatedRenderingPeripheralQuality FoveatedRenderingPeripheralQuality;

	UPROPERTY(EditAnywhere, config, Category = DirectPreview, meta = (
		ConsoleVariable = "wvr.DirectPreview.ConnectType", DisplayName = "Connect Type",
		ToolTip = "How to connect the HMD."))
	EDirectPreviewConnectType DirectPreviewConnectType;

	UPROPERTY(EditAnywhere, config, Category = DirectPreview, meta = (
		ConsoleVariable = "wvr.DirectPreview.DeviceWiFiIP", DisplayName = "Device Wi-Fi IP",
		ToolTip = "HMD's Wi-Fi IP."))
	FString sDirectPreviewDeviceWiFiIP;

	UPROPERTY(EditAnywhere, config, Category = DirectPreview, meta = (
		ConsoleVariable = "wvr.DirectPreview.EnablePreviewImage", DisplayName = "Enable Preview Image",
		ToolTip = "Enable to preview image in the HMD."))
	bool bDirectPreviewEnablePreviewImage;

	UPROPERTY(EditAnywhere, config, Category = DirectPreview, meta = (
		ConsoleVariable = "wvr.DirectPreview.UpdateFrequency", DisplayName = "Update Frequency",
		ToolTip = "Frequency to send images to the HMD."))
	EDirectPreviewUpdateFrequency DirectPreviewUpdateFrequency;

	UPROPERTY(EditAnywhere, config, Category = DirectPreview, meta = (
		ConsoleVariable = "wvr.DirectPreview.RegularlySaveImages", DisplayName = "Regularly Save Images",
		ToolTip = "Enable to save images regulary."))
	bool bDirectPreviewEnableRegularlySaveImages;

	UPROPERTY(EditAnywhere, config, Category = Performance, meta = (
		ConsoleVariable = "wvr.AdaptiveQuality", DisplayName = "Enable Adaptive Quality",
		ToolTip = "AdaptiveQuality help control the CPU and GPU frequency to save power.  Default is enabled.  If turn it off, the CPU and GPU will run at full speed."))
		bool bAdaptiveQuality;

public:
	//~ Begin UObject Interface
	virtual void PostInitProperties() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
};
