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

#include "WaveVRAPIWrapper.h"

// Define function pointer for explicit dll linking
#if WITH_EDITOR
#include "DirectPreview/WaveVR_DirectPreview.h"
typedef DP_InitError (*_WVR_Init)(DP_ConnectType type, const char* IP, bool enablePreview, bool dllToFile, bool saveImage);
typedef void (*_WVR_SetPrintCallback)(PrintLog callback);
typedef bool(*_WVR_SetRenderImageHandle)(void* textureHandle);
#else
typedef WVR_InitError (*_WVR_Init)(WVR_AppType type);
#endif
typedef void (*_WVR_Quit)();
typedef void (*_WVR_GetSyncPose)(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount);
typedef bool (*_WVR_GetInputButtonState)(WVR_DeviceType type, WVR_InputId id);
typedef bool (*_WVR_GetInputTouchState)(WVR_DeviceType type, WVR_InputId id);
typedef WVR_Axis_t (*_WVR_GetInputAnalogAxis)(WVR_DeviceType type, WVR_InputId id);
typedef bool (*_WVR_IsDeviceConnected)(WVR_DeviceType type);
typedef uint32_t (*_WVR_GetParameters)(WVR_DeviceType type, const char* Parameter, char* retValue, uint32_t bufferSize);
typedef WVR_NumDoF(*_WVR_GetDegreeOfFreedom)(WVR_DeviceType type);
typedef float (*_WVR_GetDeviceBatteryPercentage)(WVR_DeviceType type);
typedef bool (*_WVR_PollEventQueue)(WVR_Event_t* event);

typedef bool (*_WVR_GetRenderProps)(WVR_RenderProps_t* props);
typedef bool (*_WVR_SetInputRequest)(WVR_DeviceType type, const WVR_InputAttribute* request, uint32_t size);
typedef bool (*_WVR_SetInteractionMode)(WVR_InteractionMode mode);
typedef WVR_InteractionMode (*_WVR_GetInteractionMode)();
typedef bool (*_WVR_SetGazeTriggerType)(WVR_GazeTriggerType type);
typedef WVR_GazeTriggerType (*_WVR_GetGazeTriggerType)();
typedef void (*_WVR_SetNeckModelEnabled)(bool enabled);
typedef WVR_DeviceType (*_WVR_GetDefaultControllerRole)();
typedef void (*_WVR_SetArmSticky)(bool stickyArm);
typedef void (*_WVR_SetArmModel)(WVR_SimulationType type);
typedef void (*_WVR_InAppRecenter)(WVR_RecenterType recenterType);
typedef void (*_WVR_SetArenaVisible)(WVR_ArenaVisible config);
typedef bool (*_WVR_StartCamera)(WVR_CameraInfo_t *info);
typedef void (*_WVR_StopCamera)();
typedef bool (*_WVR_GetCameraFrameBuffer)(uint8_t *pframebuffer, uint32_t frameBufferSize);
typedef bool (*_WVR_RequestScreenshot)(uint32_t width, uint32_t height, WVR_ScreenshotMode mode, const char* filename);
typedef uint32_t (*_WVR_GetWaveRuntimeVersion)();
typedef bool (*_WVR_GetInputMappingPair)(WVR_DeviceType type, WVR_InputId destination, WVR_InputMappingPair* pair);
typedef uint32_t (*_WVR_GetInputMappingTable)(WVR_DeviceType type, WVR_InputMappingPair* table, uint32_t size);
typedef bool (*_WVR_IsInputFocusCapturedBySystem)();
typedef void (*_WVR_TriggerVibration)(WVR_DeviceType type, WVR_InputId id, uint32_t durationMicroSec, uint32_t frequency, WVR_Intensity intensity);

typedef WVR_RenderError (*_WVR_RenderInit)(const WVR_RenderInitParams_t* param);
typedef WVR_Matrix4f_t (*_WVR_GetTransformFromEyeToHead)(WVR_Eye eye, WVR_NumDoF dof);
typedef WVR_SubmitError (*_WVR_SubmitFrame)(WVR_Eye eye, const WVR_TextureParams_t *param, const WVR_PoseState_t* pose , WVR_SubmitExtend extendMethod);
typedef WVR_TextureQueueHandle_t (*_WVR_ObtainTextureQueue)(WVR_TextureTarget target, WVR_TextureFormat format, WVR_TextureType type, uint32_t width, uint32_t height, int32_t level);
typedef uint32_t (*_WVR_GetTextureQueueLength)(WVR_TextureQueueHandle_t handle);
typedef int32_t (*_WVR_GetAvailableTextureIndex)(WVR_TextureQueueHandle_t handle);
typedef WVR_TextureParams_t (*_WVR_GetTexture)(WVR_TextureQueueHandle_t handle, int32_t index);
typedef void (*_WVR_ReleaseTextureQueue)(WVR_TextureQueueHandle_t handle);
typedef bool (*_WVR_IsRenderFoveationSupport)();
typedef void (*_WVR_RenderFoveation)(bool enable);
typedef void (*_WVR_GetRenderTargetSize)(uint32_t* width, uint32_t * height);
typedef void (*_WVR_GetClippingPlaneBoundary)(WVR_Eye eye, float * left, float * right, float * top, float * bottom);
typedef void (*_WVR_PreRenderEye)(WVR_Eye eye, const WVR_TextureParams_t *textureParam, const WVR_RenderFoveationParams_t* foveatedParam);

typedef WVR_OverlayError (*_WVR_GenOverlay)(int32_t *overlayId);
typedef WVR_OverlayError (*_WVR_DelOverlay)(int32_t overlayId);
typedef WVR_OverlayError (*_WVR_SetOverlayTextureId)(int32_t overlayId, const WVR_OverlayTexture_t *texture);
typedef WVR_OverlayError (*_WVR_SetOverlayFixedPosition)(int32_t overlayId, const WVR_OverlayPosition_t *position);
typedef WVR_OverlayError (*_WVR_ShowOverlay)(int32_t overlayId);
typedef WVR_OverlayError (*_WVR_HideOverlay)(int32_t overlayId);
typedef bool (*_WVR_IsOverlayValid)(int32_t overlayId);

#if WITH_EDITOR
#define FUNC_PTR(name) \
	static _WVR_ ## name funcPtr = nullptr; \
	if (mDllHandle != nullptr && funcPtr == nullptr) \
	{ \
		FString procName = "WVR_" #name "_S"; \
		funcPtr = (_WVR_ ## name) FPlatformProcess::GetDllExport(mDllHandle, *procName); \
	}
#else
#define FUNC_PTR(name) \
	static _WVR_ ## name funcPtr = nullptr; \
	if (mDllHandle != nullptr && funcPtr == nullptr) \
	{ \
		FString procName = "WVR_" #name; \
		funcPtr = (_WVR_ ## name) FPlatformProcess::GetDllExport(mDllHandle, *procName); \
	}
#endif

#define FUNC_RUN_6(A, B, C, D, E, F) \
	if (funcPtr != nullptr) \
		return funcPtr(A, B, C, D, E, F)

#define FUNC_RUN_5(A, B, C, D, E) \
	if (funcPtr != nullptr) \
		return funcPtr(A, B, C, D, E)

#define FUNC_RUN_4(A, B, C, D) \
	if (funcPtr != nullptr) \
		return funcPtr(A, B, C, D)

#define FUNC_RUN_3(A, B, C) \
	if (funcPtr != nullptr) \
		return funcPtr(A, B, C)

#define FUNC_RUN_2(A, B) \
	if (funcPtr != nullptr) \
		return funcPtr(A, B)

#define FUNC_RUN_1(A) \
	if (funcPtr != nullptr) \
		return funcPtr(A)

#define FUNC_RUN() \
	if (funcPtr != nullptr) \
		return funcPtr()
