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

#include <string>
#include <vector>

#include "wvr.h"
#include "wvr_device.h"
#include "wvr_events.h"
#include "wvr_render.h"
#include "wvr_projection.h"
#include "wvr_types.h"
#include "wvr_system.h"
#include "wvr_overlay.h"
#include "wvr_arena.h"
#include "wvr_camera.h"
#include "wvr_hand.h"

#define WVR() FWaveVRAPIWrapper::GetInstance()

class WAVEVR_API FWaveVRAPIWrapper {

	friend class FWaveVRHMD;
	friend class FWaveVRPlugin;

public:
	void Tick();

public:
	static inline WVR_Matrix4f_t GetIdentityMatrix4f() {
		WVR_Matrix4f_t m = {};
		m.m[0][0] = 1;
		m.m[1][1] = 1;
		m.m[2][2] = 1;
		m.m[3][3] = 1;
		return m;
	}

	static inline WVR_Matrix4f_t GetTranslationMatrix4f(float x, float y, float z) {
		WVR_Matrix4f_t m = GetIdentityMatrix4f();
		m.m[0][3] = x;
		m.m[1][3] = y;
		m.m[2][3] = z;
		return m;
	}

	static inline WVR_DevicePosePair_t GetBasicPosePair() {
		WVR_DevicePosePair_t pose = {};
		pose.type = WVR_DeviceType_HMD;
		pose.pose = GetBasicPose(WVR_PoseOriginModel_OriginOnHead);
		return pose;
	}

	static inline WVR_PoseState_t GetBasicPose(WVR_PoseOriginModel originModel) {
		WVR_PoseState_t pose = {};
		pose.isValidPose = true;
		pose.poseMatrix = GetIdentityMatrix4f();
		pose.originModel = originModel;
		return pose;
	}

public:
	virtual uint64_t GetSupportedFeatures() {
		return (
			(uint64_t)WVR_SupportedFeature::WVR_SupportedFeature_PassthroughImage |
			(uint64_t)WVR_SupportedFeature::WVR_SupportedFeature_PassthroughOverlay |
			(uint64_t)WVR_SupportedFeature::WVR_SupportedFeature_HandTracking |
			(uint64_t)WVR_SupportedFeature::WVR_SupportedFeature_HandGesture
			);
	}
	virtual WVR_InitError Init(WVR_AppType type) { return WVR_InitError_NotInitialized; }
	virtual void Quit() { return ; }
	virtual void GetSyncPose(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount) { if (PoseCount > 0 && retPose != nullptr) retPose[0] = GetBasicPosePair(); }
	virtual void GetPoseState(WVR_DeviceType type, WVR_PoseOriginModel originModel, uint32_t predictedMilliSec, WVR_PoseState_t *poseState) { if (poseState != nullptr) *poseState = GetBasicPose(originModel); }
	virtual void SetPosePredictEnabled(WVR_DeviceType type, bool enabled_position_predict, bool enabled_rotation_predict) {}
	virtual bool GetInputButtonState(WVR_DeviceType type, WVR_InputId id) { return false; }
	virtual bool GetInputTouchState(WVR_DeviceType type, WVR_InputId id) { return false; }
	virtual WVR_Axis_t GetInputAnalogAxis(WVR_DeviceType type, WVR_InputId id) { return WVR_Axis_t(); }
	virtual bool IsDeviceConnected(WVR_DeviceType type) { return false; }
	virtual uint32_t GetParameters(WVR_DeviceType type, const char* param, char* ret, uint32_t bufferSize) { return 0; }
	virtual WVR_NumDoF GetDegreeOfFreedom(WVR_DeviceType type) { return WVR_NumDoF(0); }
	virtual float GetDeviceBatteryPercentage(WVR_DeviceType type) { return -0.1f; }
	virtual bool PollEventQueue(WVR_Event_t* event) { return false; }
	virtual bool GetRenderProps(WVR_RenderProps_t* props) { return false; }
	virtual bool SetInputRequest(WVR_DeviceType type, const WVR_InputAttribute* request, uint32_t size) { return false; }
	virtual bool SetInteractionMode(WVR_InteractionMode mode) { return false; }
	virtual WVR_InteractionMode GetInteractionMode() { return WVR_InteractionMode::WVR_InteractionMode_Controller; }
	virtual bool SetGazeTriggerType(WVR_GazeTriggerType type) { return false; }
	virtual WVR_GazeTriggerType GetGazeTriggerType() { return WVR_GazeTriggerType::WVR_GazeTriggerType_Timeout; }
	virtual void SetNeckModelEnabled(bool enabled) {}
	virtual WVR_DeviceType GetDefaultControllerRole() { return WVR_DeviceType::WVR_DeviceType_Invalid; }
	virtual void SetArmSticky(bool stickyArm) {}
	virtual void SetArmModel(WVR_SimulationType type) {}
	virtual void InAppRecenter(WVR_RecenterType recenterType) {}
	virtual void SetArenaVisible(WVR_ArenaVisible config) {}
	virtual bool StartCamera(WVR_CameraInfo_t *info) { return false; }
	virtual void StopCamera() {}
	virtual bool GetCameraFrameBuffer(uint8_t *pframebuffer, uint32_t frameBufferSize) { return false; }
	virtual bool RequestScreenshot(uint32_t width, uint32_t height, WVR_ScreenshotMode mode, const char* filename) { return false; }
	virtual uint32_t GetWaveRuntimeVersion() { return 0; }
	virtual bool GetInputMappingPair(WVR_DeviceType type, WVR_InputId destination, WVR_InputMappingPair* pair) { return false; }
	virtual uint32_t GetInputMappingTable(WVR_DeviceType type, WVR_InputMappingPair* table, uint32_t size) { return 0; }
	virtual bool IsInputFocusCapturedBySystem() { return false; }
	virtual void TriggerVibration(WVR_DeviceType type, WVR_InputId id = WVR_InputId_Max, uint32_t durationMicroSec = 65535, uint32_t frequency = 1, WVR_Intensity intensity = WVR_Intensity_Normal) {}
	virtual void SetParameters(WVR_DeviceType type, const char *pchValue) {}

	/* Gesture and Hand Tracking */
	virtual WVR_Result StartHandGesture() { return WVR_Result::WVR_Success; }
	virtual void StopHandGesture() {}
	virtual WVR_Result GetHandGestureData(WVR_HandGestureData *data) { *data = gestureData; return WVR_Result::WVR_Success; }
	virtual WVR_Result StartHandTracking() { return WVR_Result::WVR_Success; }
	virtual void StopHandTracking() {}
	virtual WVR_Result GetHandTrackingData(WVR_HandTrackingData *data) { *data = handTrackingData; return WVR_Result::WVR_Success; }


private:
	/* Gesture */
	WVR_HandGestureData gestureData;
	WVR_HandTrackingData handTrackingData;

public:
	virtual WVR_RenderError RenderInit(const WVR_RenderInitParams_t* param) { return WVR_RenderError(0); }
	virtual WVR_Matrix4f_t GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof = WVR_NumDoF_6DoF) { return GetTranslationMatrix4f(eye == WVR_Eye_Left ? -0.03 : 0.03, 0.10, -0.10); }
	virtual WVR_SubmitError SubmitFrame(WVR_Eye eye, const WVR_TextureParams_t *param, const WVR_PoseState_t* pose = NULL, WVR_SubmitExtend extendMethod = WVR_SubmitExtend_Default) { return WVR_SubmitError(0); }
	virtual WVR_TextureQueueHandle_t ObtainTextureQueue(WVR_TextureTarget target, WVR_TextureFormat format, WVR_TextureType type, uint32_t width, uint32_t height, int32_t level) { return WVR_TextureQueueHandle_t(); }
	virtual uint32_t GetTextureQueueLength(WVR_TextureQueueHandle_t handle) { return 1; }
	virtual int32_t GetAvailableTextureIndex(WVR_TextureQueueHandle_t handle) { return 0; }
	virtual WVR_TextureParams_t GetTexture(WVR_TextureQueueHandle_t handle, int32_t index) { return WVR_TextureParams_t(); }
	virtual void ReleaseTextureQueue(WVR_TextureQueueHandle_t handle) {}
	virtual bool IsRenderFoveationSupport() { return false; }
	virtual void RenderFoveation(bool enable) {}
	virtual bool IsAdaptiveQualityEnabled() { return false; }
	virtual bool EnableAdaptiveQuality(bool enable) { return false; }
	virtual void GetRenderTargetSize(uint32_t* width, uint32_t * height) { *width = 1024; *height = 1024; }
	virtual void GetClippingPlaneBoundary(WVR_Eye eye, float * left, float * right, float * top, float * bottom) { *right = *top = 1; *left = *bottom = -1; }
	virtual void PreRenderEye(WVR_Eye eye, const WVR_TextureParams_t *textureParam, const WVR_RenderFoveationParams_t* foveatedParam = NULL) {}

	/* Overlay */
	virtual WVR_OverlayError GenOverlay(int32_t *overlayId) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError DelOverlay(int32_t overlayId) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError SetOverlayTextureId(int32_t overlayId, const WVR_OverlayTexture_t *texture) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError SetOverlayFixedPosition(int32_t overlayId, const WVR_OverlayPosition_t *position) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError ShowOverlay(int32_t overlayId) { return WVR_OverlayError_None; }
	virtual WVR_OverlayError HideOverlay(int32_t overlayId) { return WVR_OverlayError_None; }
	virtual bool IsOverlayValid(int32_t overlayId) { return true; }

	/* WVR internal API */
	virtual bool IsATWActive() { return false; }
	virtual void SetATWActive(bool isActive, void *anativeWindow = nullptr) {}
	virtual void PauseATW() {}  // New Api to replace SetATWActive(false)
	virtual void ResumeATW() {}  // New Api to replace SetATWActive(true)
	virtual void OnDisable() {}  // New Api to replace SetATWActive(false) on Unity3D OnDisable
	virtual void OnApplicationQuit() {}  // New Api to handle OnApplicationQuit case
	virtual void SetRenderThreadId(int tid) {}
	virtual WVR_TextureQueueHandle_t StoreRenderTextures(void ** texturesIDs, int size, bool eEye) { return WVR_TextureQueueHandle_t(); }
	virtual void GetStencilMesh(WVR_Eye eye, uint32_t* vertexCount, uint32_t* triangleCount, uint32_t floatArrayCount, float* vertexData, uint32_t intArrayCount, int* indexData) {}

	virtual void SetFocusedController(WVR_DeviceType focusedController) { focusController = focusedController; }

	virtual WVR_DeviceType GetFocusedController() { return focusController; }
private:
	WVR_DeviceType focusController;

public:
	/* WVR internal API - Resource Wrapper */
	virtual std::string GetStringBySystemLanguage(std::string str) { return std::string(""); }
	virtual std::string GetStringByLanguage(std::string str, std::string lang, std::string country) { return std::string(""); }
	virtual std::string GetSystemLanguage() { return std::string(""); }
	virtual std::string GetSystemCountry() { return std::string(""); }

	/* WVR internal API - OEM Config*/
	virtual std::string GetOEMConfigRawData(std::string key) { return std::string(""); }

	/* WVR internal API - Controller Loader*/
	virtual std::string DeployRenderModelAssets(int deviceIndex, std::string renderModelName) { return std::string(""); }
	virtual std::string GetRootRelativePath() { return std::string(""); }

	/* WVR internal API - Permission Manager */
	virtual bool RequestPermissions(std::vector<std::string> permissions) { return false; }
	virtual bool RequestUsbPermission() { return false; }
	virtual bool IsPermissionGranted(std::string permission) { return false; }
	virtual bool ShouldPermissionGranted(std::string permission) { return false; }
	virtual bool ShowDialogOnVRScene() { return false; }

public:
	virtual bool LoadLibraries() { return false; }
	virtual void UnLoadLibraries() {}

public:
	inline static FWaveVRAPIWrapper * GetInstance()
	{
		return instance;
	}

	virtual ~FWaveVRAPIWrapper();

protected:
	FWaveVRAPIWrapper();

private:
	static void SetInstance(FWaveVRAPIWrapper * instance);

private:
	static FWaveVRAPIWrapper * instance;
};
