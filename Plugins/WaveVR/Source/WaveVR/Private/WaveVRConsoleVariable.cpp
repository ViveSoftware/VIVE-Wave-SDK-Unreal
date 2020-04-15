// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRPrivatePCH.h"

#include "HAL/IConsoleManager.h"

// https://docs.unrealengine.com/en-us/Programming/Development/Tools/ConsoleManager

/*
static TAutoConsoleVariable<int32> CVarTemplate(
	TEXT("wvr."),
	1,  // Default value
	TEXT("1. Enable .\n")
	TEXT("0. Disable it.\n")
	TEXT("note, or orther description.\n"),
	ECVF_RenderThreadSafe);
*/

/****************************************************
 *
 * Console Variable: HMD
 *
 ****************************************************/

static TAutoConsoleVariable<int32> CVarEyeTrackingEnable(
	TEXT("wvr.EyeTracking.enable"),
	/*default value*/ 1,
	TEXT("1. Enable the eye tracking feature.\n")
	TEXT("0. Disable it.\n")
	TEXT("If not support by device, it will be disabled.\n"),
	ECVF_RenderThreadSafe);


/****************************************************
 *
 * Console Variable: Render
 *
 ****************************************************/

static TAutoConsoleVariable<int32> CVarRenderMaskEnable(
	TEXT("wvr.RenderMask.enable"),
	/*default value*/ 1,
	TEXT("1. Enable the RenderMask to skip the rendering on invisible area.  Will not work if HMD LateUpdate is enabled.\n")
	TEXT("0. RenderMask will disable.  Render to whole output texture.\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarFoveatedRenderingEnable(
	TEXT("wvr.FoveatedRendering.enable"),
	/*default value*/ 1,
	TEXT("Description here.\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarFoveatedRenderingPeripheralFOV(
	TEXT("wvr.FoveatedRendering.peripheralFOV"),
	/*default value*/ 33,  // Magic number means use default.
	TEXT("Description here.\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarFoveatedRenderingPeripheralQuality(
	TEXT("wvr.FoveatedRendering.peripheralQuality"),
	/*default value*/ 2,
	TEXT("Description here.\n")
	TEXT("  0: low quality\n")
	TEXT("  1: middle quality\n")
	TEXT("  2: high quality (default)"),
	ECVF_RenderThreadSafe);

/****************************************************
 *
 * Console Variable: Direct Preview
 *
 ****************************************************/

static TAutoConsoleVariable<int32> CVarConnectType(
	TEXT("wvr.DirectPreview.ConnectType"),
	/*default value*/ 1,
	TEXT("0. USB.\n")
	TEXT("1. Wi-Fi.\n"),
	ECVF_SetByProjectSetting);

static TAutoConsoleVariable<FString> CVarDeviceWiFiIP(
	TEXT("wvr.DirectPreview.DeviceWiFiIP"),
	/*default value*/ TEXT(""),
	TEXT("HMD's Wi-Fi IP.\n"),
	ECVF_SetByProjectSetting);

static TAutoConsoleVariable<int32> CVarEnablePreviewImage(
	TEXT("wvr.DirectPreview.EnablePreviewImage"),
	/*default value*/ 1,
	TEXT("1. Enable to preview image in the HMD.\n")
	TEXT("0. Disable to preview image in the HMD.\n"),
	ECVF_SetByProjectSetting);

static TAutoConsoleVariable<int32> CVarUpdateFrequency(
	TEXT("wvr.DirectPreview.UpdateFrequency"),
	/*default value*/ 4,
	TEXT("0. Runtime defined.\n")
	TEXT("1. FPS_15.\n")
	TEXT("2. FPS_30.\n")
	TEXT("3. FPS_45.\n")
	TEXT("4. FPS_60.\n")
	TEXT("5. FPS_75.\n"),
	ECVF_SetByProjectSetting);

static TAutoConsoleVariable<int32> CVarRegularlySaveImages(
	TEXT("wvr.DirectPreview.RegularlySaveImages"),
	/*default value*/ 0,
	TEXT("1. Enable to save images regulary.\n")
	TEXT("0. Disable to save images regulary.\n"),
	ECVF_SetByProjectSetting);

/****************************************************
 *
 * Console Variable: Performance
 *
 ****************************************************/

static TAutoConsoleVariable<int32> CVarAdaptiveQuality(
	TEXT("wvr.AdaptiveQuality"),
	/*default value*/ 1,
	TEXT("1. Adjuest CPU/GPU performance by the loading.\n")
	TEXT("0. Always use full speed CPU and GPU.\n"),
	ECVF_RenderThreadSafe);
