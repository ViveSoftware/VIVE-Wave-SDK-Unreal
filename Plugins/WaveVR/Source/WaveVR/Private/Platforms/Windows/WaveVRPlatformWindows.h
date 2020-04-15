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

#include "Platforms/WaveVRAPIWrapper.h"
#include "Platforms/DllLoader.h"

class WAVEVR_API FWaveVRPlatformWindows : public FWaveVRAPIWrapper
{
public:
	FWaveVRPlatformWindows();
	virtual ~FWaveVRPlatformWindows();

public:
	bool LoadLibraries() override;
	void UnLoadLibraries() override;

public:
#if !WITH_EDITOR
	virtual WVR_InitError Init(WVR_AppType type) override;
#endif
	virtual void Quit() override;
	virtual void GetSyncPose(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount) override;
	virtual bool GetInputButtonState(WVR_DeviceType type, WVR_InputId id) override;
	virtual bool GetInputTouchState(WVR_DeviceType type, WVR_InputId id) override;
	virtual WVR_Axis_t GetInputAnalogAxis(WVR_DeviceType type, WVR_InputId id) override;
	virtual bool IsDeviceConnected(WVR_DeviceType type) override;
	virtual uint32_t GetParameters(WVR_DeviceType type, const char* param, char* ret, uint32_t bufferSize) override;
	virtual WVR_NumDoF GetDegreeOfFreedom(WVR_DeviceType type) override;
	virtual float GetDeviceBatteryPercentage(WVR_DeviceType type) override;
	virtual bool PollEventQueue(WVR_Event_t* event) override;

	virtual bool GetRenderProps(WVR_RenderProps_t* props) override;
	virtual bool SetInputRequest(WVR_DeviceType type, const WVR_InputAttribute* request, uint32_t size) override;
	virtual bool SetInteractionMode(WVR_InteractionMode mode) override;
	virtual WVR_InteractionMode GetInteractionMode() override;
	virtual bool SetGazeTriggerType(WVR_GazeTriggerType type) override;
	virtual WVR_GazeTriggerType GetGazeTriggerType() override;
	virtual void SetNeckModelEnabled(bool enabled) override;
	virtual WVR_DeviceType GetDefaultControllerRole() override;
	virtual void SetArmSticky(bool stickyArm) override;
	virtual void SetArmModel(WVR_SimulationType type) override;
	virtual void InAppRecenter(WVR_RecenterType recenterType) override;
	virtual void SetArenaVisible(WVR_ArenaVisible config) override;
	virtual bool StartCamera(WVR_CameraInfo_t *info) override;
	virtual void StopCamera() override;
	virtual bool GetCameraFrameBuffer(uint8_t *pframebuffer, uint32_t frameBufferSize) override;
	virtual bool RequestScreenshot(uint32_t width, uint32_t height, WVR_ScreenshotMode mode, const char* filename) override;
	virtual uint32_t GetWaveRuntimeVersion() override;
	virtual bool GetInputMappingPair(WVR_DeviceType type, WVR_InputId destination, WVR_InputMappingPair* pair) override;
	virtual uint32_t GetInputMappingTable(WVR_DeviceType type, WVR_InputMappingPair* table, uint32_t size) override;
	virtual bool IsInputFocusCapturedBySystem() override;
	virtual void TriggerVibration(WVR_DeviceType type, WVR_InputId id = WVR_InputId_Max, uint32_t durationMicroSec = 65535, uint32_t frequency = 1, WVR_Intensity intensity = WVR_Intensity_Normal) override;

	virtual WVR_RenderError RenderInit(const WVR_RenderInitParams_t* param) override;
	virtual WVR_Matrix4f_t GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof = WVR_NumDoF_6DoF) override;
	virtual WVR_SubmitError SubmitFrame(WVR_Eye eye, const WVR_TextureParams_t *param, const WVR_PoseState_t* pose = NULL, WVR_SubmitExtend extendMethod = WVR_SubmitExtend_Default) override;
	virtual WVR_TextureQueueHandle_t ObtainTextureQueue(WVR_TextureTarget target, WVR_TextureFormat format, WVR_TextureType type, uint32_t width, uint32_t height, int32_t level) override;
	virtual uint32_t GetTextureQueueLength(WVR_TextureQueueHandle_t handle) override;
	virtual int32_t GetAvailableTextureIndex(WVR_TextureQueueHandle_t handle) override;
	virtual WVR_TextureParams_t GetTexture(WVR_TextureQueueHandle_t handle, int32_t index) override;
	virtual void ReleaseTextureQueue(WVR_TextureQueueHandle_t handle) override;
	virtual bool IsRenderFoveationSupport() override;
	virtual void RenderFoveation(bool enable) override;

	virtual void GetRenderTargetSize(uint32_t* width, uint32_t * height) override;
	virtual void GetClippingPlaneBoundary(WVR_Eye eye, float * left, float * right, float * top, float * bottom) override;
	virtual void PreRenderEye(WVR_Eye eye, const WVR_TextureParams_t *textureParam, const WVR_RenderFoveationParams_t* foveatedParam = NULL) override;

	/* Overlay */
	virtual WVR_OverlayError GenOverlay(int32_t *overlayId) override;
	virtual WVR_OverlayError DelOverlay(int32_t overlayId) override;
	virtual WVR_OverlayError SetOverlayTextureId(int32_t overlayId, const WVR_OverlayTexture_t *texture) override;
	virtual WVR_OverlayError SetOverlayFixedPosition(int32_t overlayId, const WVR_OverlayPosition_t *position) override;
	virtual WVR_OverlayError ShowOverlay(int32_t overlayId) override;
	virtual WVR_OverlayError HideOverlay(int32_t overlayId) override;
	virtual bool IsOverlayValid(int32_t overlayId) override;

private:
	FDllLoader DllLoader;
	void * mDllHandle = nullptr;
};
