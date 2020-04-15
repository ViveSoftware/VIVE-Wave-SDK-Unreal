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
#include "Platforms/DLLLoader.h"

#if WITH_EDITOR
#include "DirectPreview/WaveVR_DirectPreview.h"
#endif

class WAVEVR_API WaveVRDirectPreview : public FWaveVRAPIWrapper
{
public:
	WaveVRDirectPreview();
	virtual ~WaveVRDirectPreview();

	bool LoadLibraries() override;
	void UnLoadLibraries() override;

public:
	virtual WVR_InitError Init(WVR_AppType type) override { return WVR_InitError_None; }
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
	virtual void GetRenderTargetSize(uint32_t* width, uint32_t* height) override;
	virtual void GetClippingPlaneBoundary(WVR_Eye eye, float* left, float* right, float* top, float* bottom) override;
	virtual WVR_Matrix4f_t GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof) override;
	virtual bool GetRenderProps(WVR_RenderProps_t* props) override;

public:
	static bool IsVRPreview();
	static bool IsDirectPreview();
	bool HookVRPreview();
	bool sendRTTexture(FRHICommandListImmediate& RHICmdList, void* NativeResource);
	void ExportTexture(FRHICommandListImmediate& RHICmdList, FRHITexture2D* TexRef2D);

#if WITH_EDITOR
private:
	DP_InitError SimulatorInit(DP_ConnectType type, const char* IP, bool enablePreview, bool dllToFile, bool saveImage);
	void SetPrintCallback(PrintLog callback);
	void ReportSimError(DP_InitError error);
	bool isTimeToUpdate();
	DP_InitError ReadSettingsAndInit();
	int mFPS = 60;
	int mEnablePreviewImage = 0;

private:
	static void DllLog(const char* msg);
#endif

	FDllLoader DllLoader;
	void * mDllHandle = nullptr;
	bool isInitialized = false;
};
