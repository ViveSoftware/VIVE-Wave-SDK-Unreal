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

#include "Platforms/Windows/WaveVRPlatformWindows.h"
#include "Platforms/DLLFunctionPointer.h"

DEFINE_LOG_CATEGORY_STATIC(WVR_Windows, Log, All);

#if !WITH_EDITOR
#ifdef FUNC_CHECK
#undef FUNC_CHECK
#endif
#define FUNC_CHECK() \
	if (funcPtr == nullptr) \
		UE_LOG(WVR_Windows, Error, TEXT("Failed to load Windows library."));
#endif

FWaveVRPlatformWindows::FWaveVRPlatformWindows()
	:DllLoader(FString("wave_api.dll"))
{
	LOG_FUNC();
}

FWaveVRPlatformWindows::~FWaveVRPlatformWindows()
{
	mDllHandle = nullptr;
}

bool FWaveVRPlatformWindows::LoadLibraries()
{
	LOG_FUNC();
	bool ret = DllLoader.LoadExternalDLL();
	if (mDllHandle == nullptr && DllLoader.GetHandle() != nullptr) {
		mDllHandle = DllLoader.GetHandle();
	}
	return ret;
}

void FWaveVRPlatformWindows::UnLoadLibraries()
{
	LOG_FUNC();
	DllLoader.UnloadExternalDLL();
}

#if !WITH_EDITOR
WVR_InitError FWaveVRPlatformWindows::Init(WVR_AppType type)
{
	LOG_FUNC();
	FUNC_PTR(Init);
	FUNC_RUN_1(type);
	FUNC_CHECK();
	return WVR_InitError_NotInitialized;
}
#endif

void FWaveVRPlatformWindows::Quit()
{
	LOG_FUNC();
	FUNC_PTR(Quit);
	FUNC_RUN();
	FUNC_CHECK();
}

void FWaveVRPlatformWindows::GetSyncPose(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount)
{
	LOG_FUNC();
	FUNC_PTR(GetSyncPose);
	FUNC_RUN_3(originModel, retPose, PoseCount);
	FUNC_CHECK();
}

bool FWaveVRPlatformWindows::GetInputButtonState(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	FUNC_PTR(GetInputButtonState);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	return false; //failure
}

bool FWaveVRPlatformWindows::GetInputTouchState(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	FUNC_PTR(GetInputTouchState);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	return false;
}

WVR_Axis_t FWaveVRPlatformWindows::GetInputAnalogAxis(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	FUNC_PTR(GetInputAnalogAxis);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	WVR_Axis_t ret;
	ret.x=ret.y=0.0f;
	return ret; //failure
}

bool FWaveVRPlatformWindows::IsDeviceConnected(WVR_DeviceType type)
{
	LOG_FUNC();
	FUNC_PTR(IsDeviceConnected);
	FUNC_RUN_1(type);
	FUNC_CHECK();
	return false; //failure
}

uint32_t FWaveVRPlatformWindows::GetParameters(WVR_DeviceType type, const char* Parameter, char* retValue, uint32_t bufferSize)
{
	LOG_FUNC();
	FUNC_PTR(GetParameters);
	FUNC_RUN_4(type, Parameter, retValue, bufferSize);
	FUNC_CHECK();
	return 0; //failure
}

WVR_NumDoF FWaveVRPlatformWindows::GetDegreeOfFreedom(WVR_DeviceType type)
{
	LOG_FUNC();
	FUNC_PTR(GetDegreeOfFreedom);
	FUNC_RUN_1(type);
	FUNC_CHECK();
	return WVR_NumDoF_3DoF; //failure
}

float FWaveVRPlatformWindows::GetDeviceBatteryPercentage(WVR_DeviceType WVR_DeviceType)
{
	LOG_FUNC();
	FUNC_PTR(GetDeviceBatteryPercentage);
	FUNC_RUN_1(WVR_DeviceType);
	FUNC_CHECK();
	return 0; //failure
}

bool FWaveVRPlatformWindows::PollEventQueue(WVR_Event_t* event)
{
	// LOG_FUNC();  // too noisy
	FUNC_PTR(PollEventQueue);
	FUNC_RUN_1(event);
	FUNC_CHECK();
	return false; //failure
}

bool FWaveVRPlatformWindows::GetRenderProps(WVR_RenderProps_t* props) {
	LOG_FUNC();
	FUNC_PTR(GetRenderProps);
	FUNC_RUN_1(props);
	FUNC_CHECK();
	return false; //failure
}

bool FWaveVRPlatformWindows::SetInputRequest(WVR_DeviceType type, const WVR_InputAttribute* request, uint32_t size) {
	LOG_FUNC();
	FUNC_PTR(SetInputRequest);
	FUNC_RUN_3(type, request, size);
	FUNC_CHECK();
	return false; //failure
}

bool FWaveVRPlatformWindows::SetInteractionMode(WVR_InteractionMode mode) {
	LOG_FUNC();
	FUNC_PTR(SetInteractionMode);
	FUNC_RUN_1(mode);
	FUNC_CHECK();
	return false; //failure
}

WVR_InteractionMode FWaveVRPlatformWindows::GetInteractionMode() {
	LOG_FUNC();
	FUNC_PTR(GetInteractionMode);
	FUNC_RUN();
	FUNC_CHECK();
	return WVR_InteractionMode::WVR_InteractionMode_SystemDefault;
}

bool FWaveVRPlatformWindows::SetGazeTriggerType(WVR_GazeTriggerType type) {
	LOG_FUNC();
	FUNC_PTR(SetGazeTriggerType);
	FUNC_RUN_1(type);
	FUNC_CHECK();
	return false; //failure
}

WVR_GazeTriggerType FWaveVRPlatformWindows::GetGazeTriggerType() {
	LOG_FUNC();
	FUNC_PTR(GetGazeTriggerType);
	FUNC_RUN();
	FUNC_CHECK();
	return WVR_GazeTriggerType::WVR_GazeTriggerType_Timeout;
}

void FWaveVRPlatformWindows::SetNeckModelEnabled(bool enabled) {
	LOG_FUNC();
	FUNC_PTR(SetNeckModelEnabled);
	FUNC_RUN_1(enabled);
	FUNC_CHECK();
}

WVR_DeviceType FWaveVRPlatformWindows::GetDefaultControllerRole() {
	LOG_FUNC();
	FUNC_PTR(GetDefaultControllerRole);
	FUNC_RUN();
	FUNC_CHECK();
	return WVR_DeviceType::WVR_DeviceType_Invalid;
}

void FWaveVRPlatformWindows::SetArmSticky(bool stickyArm) {
	LOG_FUNC();
	FUNC_PTR(SetArmSticky);
	FUNC_RUN_1(stickyArm);
	FUNC_CHECK();
}

void FWaveVRPlatformWindows::SetArmModel(WVR_SimulationType type) {
	LOG_FUNC();
	FUNC_PTR(SetArmModel);
	FUNC_RUN_1(type);
	FUNC_CHECK();
}

void FWaveVRPlatformWindows::InAppRecenter(WVR_RecenterType recenterType) {
	LOG_FUNC();
	FUNC_PTR(InAppRecenter);
	FUNC_RUN_1(recenterType);
	FUNC_CHECK();
}

void FWaveVRPlatformWindows::SetArenaVisible(WVR_ArenaVisible config) {
	LOG_FUNC();
	FUNC_PTR(SetArenaVisible);
	FUNC_RUN_1(config);
	FUNC_CHECK();
}

bool FWaveVRPlatformWindows::StartCamera(WVR_CameraInfo_t *info) {
	LOG_FUNC();
	FUNC_PTR(StartCamera);
	FUNC_RUN_1(info);
	FUNC_CHECK();
	return false; //failure
}
void FWaveVRPlatformWindows::StopCamera() {
	LOG_FUNC();
	FUNC_PTR(StopCamera);
	FUNC_RUN();
	FUNC_CHECK();
}

bool FWaveVRPlatformWindows::GetCameraFrameBuffer(uint8_t *pframebuffer, uint32_t frameBufferSize) {
	LOG_FUNC();
	FUNC_PTR(GetCameraFrameBuffer);
	FUNC_RUN_2(pframebuffer, frameBufferSize);
	FUNC_CHECK();
	return false; //failure
}

bool FWaveVRPlatformWindows::RequestScreenshot(uint32_t width, uint32_t height, WVR_ScreenshotMode mode, const char* filename) {
	LOG_FUNC();
	FUNC_PTR(RequestScreenshot);
	FUNC_RUN_4(width, height, mode, filename);
	FUNC_CHECK();
	return false; //failure
}

uint32_t FWaveVRPlatformWindows::GetWaveRuntimeVersion() {
	LOG_FUNC();
	FUNC_PTR(GetWaveRuntimeVersion);
	FUNC_RUN();
	FUNC_CHECK();
	return 0; //failure
}

bool FWaveVRPlatformWindows::GetInputMappingPair(WVR_DeviceType type, WVR_InputId destination, WVR_InputMappingPair* pair) {
	LOG_FUNC();
	FUNC_PTR(GetInputMappingPair);
	FUNC_RUN_3(type, destination, pair);
	FUNC_CHECK();
	return false; //failure
}

uint32_t FWaveVRPlatformWindows::GetInputMappingTable(WVR_DeviceType type, WVR_InputMappingPair* table, uint32_t size) {
	LOG_FUNC();
	FUNC_PTR(GetInputMappingTable);
	FUNC_RUN_3(type, table, size);
	FUNC_CHECK();
	return 0; //failure
}

bool FWaveVRPlatformWindows::IsInputFocusCapturedBySystem() {
	LOG_FUNC();
	FUNC_PTR(IsInputFocusCapturedBySystem);
	FUNC_RUN();
	FUNC_CHECK();
	return false; //failure
}

void FWaveVRPlatformWindows::TriggerVibration(WVR_DeviceType type, WVR_InputId id, uint32_t durationMicroSec, uint32_t frequency, WVR_Intensity intensity) {
	LOG_FUNC();
	FUNC_PTR(TriggerVibration);
	FUNC_RUN_5(type, id, durationMicroSec, frequency, intensity);
	FUNC_CHECK();
}

WVR_RenderError FWaveVRPlatformWindows::RenderInit(const WVR_RenderInitParams_t* param) {
	LOG_FUNC();
	FUNC_PTR(RenderInit);
	FUNC_RUN_1(param);
	FUNC_CHECK();
	return WVR_RenderError::WVR_RenderError_None; //failure
}

WVR_Matrix4f_t FWaveVRPlatformWindows::GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof) {
	LOG_FUNC();
	FUNC_PTR(GetTransformFromEyeToHead);
	FUNC_RUN_2(eye, dof);
	FUNC_CHECK();
	return WVR_Matrix4f_t(); //failure
}

WVR_SubmitError FWaveVRPlatformWindows::SubmitFrame(WVR_Eye eye, const WVR_TextureParams_t *param, const WVR_PoseState_t* pose, WVR_SubmitExtend extendMethod) {
	LOG_FUNC();
	FUNC_PTR(SubmitFrame);
	FUNC_RUN_4(eye, param, pose, extendMethod);
	FUNC_CHECK();
	return WVR_SubmitError::WVR_SubmitError_BufferSubmitFailed; //failure
}

WVR_TextureQueueHandle_t FWaveVRPlatformWindows::ObtainTextureQueue(WVR_TextureTarget target, WVR_TextureFormat format, WVR_TextureType type, uint32_t width, uint32_t height, int32_t level) {
	LOG_FUNC();
	FUNC_PTR(ObtainTextureQueue);
	FUNC_RUN_6(target, format, type, width, height, level);
	FUNC_CHECK();
	return WVR_TextureQueueHandle_t(); //failure
}

uint32_t FWaveVRPlatformWindows::GetTextureQueueLength(WVR_TextureQueueHandle_t handle) {
	LOG_FUNC();
	FUNC_PTR(GetTextureQueueLength);
	FUNC_RUN_1(handle);
	FUNC_CHECK();
	return 0; //failure
}

int32_t FWaveVRPlatformWindows::GetAvailableTextureIndex(WVR_TextureQueueHandle_t handle) {
	LOG_FUNC();
	FUNC_PTR(GetAvailableTextureIndex);
	FUNC_RUN_1(handle);
	FUNC_CHECK();
	return 0; //failure
}

WVR_TextureParams_t FWaveVRPlatformWindows::GetTexture(WVR_TextureQueueHandle_t handle, int32_t index) {
	LOG_FUNC();
	FUNC_PTR(GetTexture);
	FUNC_RUN_2(handle, index);
	FUNC_CHECK();
	return WVR_TextureParams_t(); //failure
}

void FWaveVRPlatformWindows::ReleaseTextureQueue(WVR_TextureQueueHandle_t handle) {
	LOG_FUNC();
	FUNC_PTR(ReleaseTextureQueue);
	FUNC_RUN_1(handle);
	FUNC_CHECK();
}

bool FWaveVRPlatformWindows::IsRenderFoveationSupport() {
	LOG_FUNC();
	FUNC_PTR(IsRenderFoveationSupport);
	FUNC_RUN();
	FUNC_CHECK();
	return false; //failure
}

void FWaveVRPlatformWindows::RenderFoveation(bool enable) {
	LOG_FUNC();
	FUNC_PTR(RenderFoveation);
	FUNC_RUN_1(enable);
	FUNC_CHECK();
}

void FWaveVRPlatformWindows::GetRenderTargetSize(uint32_t* width, uint32_t * height) {
	LOG_FUNC();
	FUNC_PTR(GetRenderTargetSize);
	FUNC_RUN_2(width, height);
	FUNC_CHECK();
}

void FWaveVRPlatformWindows::GetClippingPlaneBoundary(WVR_Eye eye, float * left, float * right, float * top, float * bottom) {
	LOG_FUNC();
	FUNC_PTR(GetClippingPlaneBoundary);
	FUNC_RUN_5(eye, left, right, top, bottom);
	FUNC_CHECK();
}

void FWaveVRPlatformWindows::PreRenderEye(WVR_Eye eye, const WVR_TextureParams_t *textureParam, const WVR_RenderFoveationParams_t* foveatedParam) {
	LOG_FUNC();
	FUNC_PTR(PreRenderEye);
	FUNC_RUN_3(eye, textureParam, foveatedParam);
	FUNC_CHECK();
}

/* Overlay */
WVR_OverlayError FWaveVRPlatformWindows::GenOverlay(int32_t *overlayId) {
	LOG_FUNC();
	FUNC_PTR(GenOverlay);
	FUNC_RUN_1(overlayId);
	FUNC_CHECK();
	return WVR_OverlayError::WVR_OverlayError_OverlayUnavailable; //failure
}

WVR_OverlayError FWaveVRPlatformWindows::DelOverlay(int32_t overlayId) {
	LOG_FUNC();
	FUNC_PTR(DelOverlay);
	FUNC_RUN_1(overlayId);
	FUNC_CHECK();
	return WVR_OverlayError::WVR_OverlayError_OverlayUnavailable; //failure
}

WVR_OverlayError FWaveVRPlatformWindows::SetOverlayTextureId(int32_t overlayId, const WVR_OverlayTexture_t *texture) {
	LOG_FUNC();
	FUNC_PTR(SetOverlayTextureId);
	FUNC_RUN_2(overlayId, texture);
	FUNC_CHECK();
	return WVR_OverlayError::WVR_OverlayError_OverlayUnavailable; //failure
}

WVR_OverlayError FWaveVRPlatformWindows::SetOverlayFixedPosition(int32_t overlayId, const WVR_OverlayPosition_t *position) {
	LOG_FUNC();
	FUNC_PTR(SetOverlayFixedPosition);
	FUNC_RUN_2(overlayId, position);
	FUNC_CHECK();
	return WVR_OverlayError::WVR_OverlayError_OverlayUnavailable; //failure
}

WVR_OverlayError FWaveVRPlatformWindows::ShowOverlay(int32_t overlayId) {
	LOG_FUNC();
	FUNC_PTR(ShowOverlay);
	FUNC_RUN_1(overlayId);
	FUNC_CHECK();
	return WVR_OverlayError::WVR_OverlayError_OverlayUnavailable; //failure
}

WVR_OverlayError FWaveVRPlatformWindows::HideOverlay(int32_t overlayId) {
	LOG_FUNC();
	FUNC_PTR(HideOverlay);
	FUNC_RUN_1(overlayId);
	FUNC_CHECK();
	return WVR_OverlayError::WVR_OverlayError_OverlayUnavailable; //failure
}

bool FWaveVRPlatformWindows::IsOverlayValid(int32_t overlayId) {
	LOG_FUNC();
	FUNC_PTR(IsOverlayValid);
	FUNC_RUN_1(overlayId);
	FUNC_CHECK();
	return false; //failure
}
