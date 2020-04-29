// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "Platforms/Editor/WaveVRDirectPreview.h"
#include "WaveVRPrivatePCH.h"
#include "Logging/LogMacros.h"
#include "Platforms/DLLFunctionPointer.h"
#include "WaveVRHMD.h"
#include <chrono>
#if WITH_EDITOR
#include "WaveVRDirectPreviewSettings.h"
#include "Settings/LevelEditorPlaySettings.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(WVRDirectPreview, Log, All);

#if 0  // Debug only
#undef LOG_FUNC
#define LOG_FUNC() UE_LOG(WVRDirectPreview, Log, TEXT("%s"), WVR_FUNCTION_STRING)
#else
#endif

#define FUNC_CHECK() \
	if (funcPtr == nullptr) \
		UE_LOG(WVRDirectPreview, Error, TEXT("Failed to load Simulator library."));

#define LOG_DP(level, Format) UE_LOG(WVRDirectPreview, level, TEXT(Format));

WaveVRDirectPreview::WaveVRDirectPreview() :
	DllLoader(FString("wvr_plugins_directpreview.dll")) {
}

WaveVRDirectPreview::~WaveVRDirectPreview() {
	mDllHandle = nullptr;
}

bool WaveVRDirectPreview::LoadLibraries() {
	LOG_FUNC();
	bool ret = DllLoader.LoadExternalDLL();
	if (mDllHandle == nullptr && DllLoader.GetHandle() != nullptr) {
		mDllHandle = DllLoader.GetHandle();
	}
	return ret;
}

void WaveVRDirectPreview::UnLoadLibraries() {
	LOG_FUNC();
	DllLoader.UnloadExternalDLL();
}

bool WaveVRDirectPreview::IsDirectPreview() {
	//LOG_FUNC();  // Too noisy
	FWaveVRHMD* HMD = FWaveVRHMD::GetInstance();
	return HMD ? HMD->IsDirectPreview() : false;
}

bool WaveVRDirectPreview::IsVRPreview()
{
	//LOG_FUNC();  // Too noisy
#if WITH_EDITOR
	ULevelEditorPlaySettings* PlaySettings = GetMutableDefault<ULevelEditorPlaySettings>();
	return PlaySettings->LastExecutedPlayModeType == PlayMode_InVR;
#else
	return false;
#endif
}

bool WaveVRDirectPreview::HookVRPreview() {
	LOG_FUNC();
	if (!GIsEditor)
		return false;

	if (!IsVRPreview())
		return false;

	if (!isInitialized) {
#if WITH_EDITOR
		SetPrintCallback(&DllLog);
		DP_InitError error = DP_InitError_None;
		error = ReadSettingsAndInit();

		if (error == DP_InitError_None)
		{
			isInitialized = true;
			LOG_DP(Log, "InVR enabled");
		}
		else
		{
			ReportSimError(error);
		}
#endif
	}
	return isInitialized;
}

#if 0
		switch (PlaySettings->LastExecutedPlayModeType)
		{
		case PlayMode_InViewPort:
			LOG_DP("InViewPort");
			break;

		case PlayMode_InEditorFloating:
			LOG_DP("InEditorFloating");
			break;

		case PlayMode_InMobilePreview:
			LOG_DP("InMobilePreview");
			break;

		case PlayMode_InTargetedMobilePreview:
			LOG_DP("InTargetedMobilePreview");
			break;

		case PlayMode_InVulkanPreview:
			LOG_DP("InVulkanPreview");
			break;

		case PlayMode_InNewProcess:
			LOG_DP("InNewProcess");
			break;

		case PlayMode_InVR:
			LOG_DP("InVR");
			bIsVRPreview = true;
			break;

		case PlayMode_Simulate:
			LOG_DP("Simulate");
			break;

		default:
			LOG_DP("<UNKNOWN>");
		}
#endif

#if WITH_EDITOR
void WaveVRDirectPreview::ReportSimError(DP_InitError error)
{
	LOG_FUNC();
	switch (error)
	{
	case DP_InitError_None:
		LOG_DP(Log, "WaveVR: DP_InitError_None");
		break;
	case DP_InitError_WSAStartUp_Failed:
		LOG_DP(Error, "WaveVR: DP_InitError_WSAStartUp_Failed");
		break;
	case DP_InitError_Already_Inited:
		LOG_DP(Error, "WaveVR: DP_InitError_Already_Inited");
		break;
	case DP_InitError_Device_Not_Found:
		LOG_DP(Error, "WaveVR: DP_InitError_Device_Not_Found");
		break;
	case DP_InitError_Can_Not_Connect_Server:
		LOG_DP(Error, "WaveVR: DP_InitError_Can_Not_Connect_Server");
		break;
	case DP_InitError_IPAddress_Null:
		LOG_DP(Error, "WaveVR: DP_InitError_IPAddress_Null");
		break;
	default:
		//LOG_DP(Error, "WaveVR: InitError default case");
		break;
	}
}

uintptr_t latestNativeResource = 0;
bool WaveVRDirectPreview::sendRTTexture(FRHICommandListImmediate& RHICmdList, void* NativeResource) {
	LOG_FUNC();
	if (!mEnablePreviewImage) return false;
	if (isInitialized) {
		if (isTimeToUpdate()) {
			void* rtPtr;
			rtPtr = NativeResource;
			if ((uintptr_t)NativeResource != latestNativeResource) {
				UE_LOG(WVRDirectPreview, Display, TEXT(" Pointer has been changed, \"%" PRIxPTR "\""), NativeResource);
				latestNativeResource = (uintptr_t)NativeResource;
			}
			static _WVR_SetRenderImageHandle funcPtr = nullptr;
			if (mDllHandle != nullptr && funcPtr == nullptr)
			{
				FString procName = "WVR_SetRenderImageHandle";
				funcPtr = (_WVR_SetRenderImageHandle)FPlatformProcess::GetDllExport(mDllHandle, *procName);
			}
			FUNC_CHECK();
			FUNC_RUN_1(rtPtr);
		}
	}
	return false;
}

void WaveVRDirectPreview::ExportTexture(FRHICommandListImmediate& RHICmdList, FRHITexture2D* TexRef2D)
{
	check(IsInRenderingThread());
	if (TexRef2D == nullptr)
	{
		return;
	}

	//	FTextureReferenceRHIRef MyTextureRHI = Texture->TextureReference.TextureReferenceRHI;
	//	FRHITexture* TexRef = MyTextureRHI->GetTextureReference()->GetReferencedTexture();
	TArray<FColor> Bitmap;
	uint32 LolStride = 0;
	char* TextureDataPtr = (char*)RHICmdList.LockTexture2D(TexRef2D, 0, EResourceLockMode::RLM_ReadOnly, LolStride, false);

	for (uint32 Row = 0; Row < TexRef2D->GetSizeY(); ++Row)
	{
		uint32* PixelPtr = (uint32*)TextureDataPtr;
		for (uint32 Col = 0; Col < TexRef2D->GetSizeX(); ++Col)
		{
			uint32 EncodedPixel = *PixelPtr;
			uint8 r = (EncodedPixel & 0x000000FF);
			uint8 g = (EncodedPixel & 0x0000FF00) >> 10;
			uint8 b = (EncodedPixel & 0x00FF0000) >> 20;
			uint8 a = (EncodedPixel & 0xFF000000) >> 30;
			FColor col = FColor(r, g, b, a);
			Bitmap.Add(FColor(b, g, r, a));
			PixelPtr++;
		}
		// move to next row:
		TextureDataPtr += LolStride;
	}
	RHICmdList.UnlockTexture2D(TexRef2D, 0, false);

	if (Bitmap.Num())
	{
		IFileManager::Get().MakeDirectory(*FPaths::ScreenShotDir(), true);
		TCHAR* ScreenFileName = TEXT("D:/UnrealExport.bmp");
		uint32 ExtendXWithMSAA = Bitmap.Num() / TexRef2D->GetSizeY();
		// Save the contents of the array to a bitmap file. (24bit only so alpha channel is dropped)
		FFileHelper::CreateBitmap(ScreenFileName, ExtendXWithMSAA, TexRef2D->GetSizeY(), Bitmap.GetData());
		UE_LOG(WVRDirectPreview, Display, TEXT("Content was saved to \"%s\""), ScreenFileName);
	}
	else
	{
		UE_LOG(WVRDirectPreview, Error, TEXT("Failed to save BMP, format or texture type is not supported"));
	}
}

DP_InitError WaveVRDirectPreview::SimulatorInit(DP_ConnectType type, const char* IP, bool enablePreview, bool dllToFile, bool saveImage)
{
	LOG_FUNC();

	FUNC_PTR(Init);
	FUNC_RUN_5(type, IP, enablePreview, dllToFile, saveImage);
	FUNC_CHECK();
	return DP_InitError_WSAStartUp_Failed;
}
#endif

void WaveVRDirectPreview::Quit()
{
	LOG_FUNC();
	isInitialized = false;
	FUNC_PTR(Quit);
	FUNC_RUN();
	FUNC_CHECK();
}

void WaveVRDirectPreview::GetSyncPose(WVR_PoseOriginModel originModel, WVR_DevicePosePair_t* retPose, uint32_t PoseCount)
{
	LOG_FUNC();
	FUNC_PTR(GetSyncPose);
	FUNC_RUN_3(originModel, retPose, PoseCount);
	FUNC_CHECK();
}

bool WaveVRDirectPreview::GetInputButtonState(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	FUNC_PTR(GetInputButtonState);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	return false; //failure
}

bool WaveVRDirectPreview::GetInputTouchState(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	FUNC_PTR(GetInputTouchState);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	return false;
}

WVR_Axis_t WaveVRDirectPreview::GetInputAnalogAxis(WVR_DeviceType type, WVR_InputId id)
{
	LOG_FUNC();
	FUNC_PTR(GetInputAnalogAxis);
	FUNC_RUN_2(type, id);
	FUNC_CHECK();
	WVR_Axis_t ret;
	ret.x=ret.y=0.0f;
	return ret; //failure
}

bool WaveVRDirectPreview::IsDeviceConnected(WVR_DeviceType type)
{
	LOG_FUNC();
	FUNC_PTR(IsDeviceConnected);
	FUNC_RUN_1(type);
	FUNC_CHECK();
	return false; //failure
}

uint32_t WaveVRDirectPreview::GetParameters(WVR_DeviceType type, const char* Parameter, char* retValue, uint32_t bufferSize)
{
	LOG_FUNC();
	FUNC_PTR(GetParameters);
	FUNC_RUN_4(type, Parameter, retValue, bufferSize);
	FUNC_CHECK();
	return 0; //failure
}

WVR_NumDoF WaveVRDirectPreview::GetDegreeOfFreedom(WVR_DeviceType type)
{
	if (!isInitialized) {
		LOG_DP(Warning, "Not init, GetDegreeOfFreedom return 3Dof.");
		return WVR_NumDoF_3DoF;
	}
	else {
		LOG_FUNC();
		FUNC_PTR(GetDegreeOfFreedom);
		FUNC_RUN_1(type);
		FUNC_CHECK();
		return WVR_NumDoF_3DoF; //failure
	}
}

float WaveVRDirectPreview::GetDeviceBatteryPercentage(WVR_DeviceType WVR_DeviceType)
{
	LOG_FUNC();
	FUNC_PTR(GetDeviceBatteryPercentage);
	FUNC_RUN_1(WVR_DeviceType);
	FUNC_CHECK();
	return 0; //failure
}

void WaveVRDirectPreview::GetRenderTargetSize(uint32_t* width, uint32_t* height)
{
	if (!isInitialized) {
		LOG_DP(Warning, "Not init, set default GetRenderTargetSize");
		*width = 1024;
		*height = 1024;
	}
	else {
		LOG_FUNC();
		FUNC_PTR(GetRenderTargetSize);
		FUNC_RUN_2(width, height);
		FUNC_CHECK();
	}
}

void WaveVRDirectPreview::GetClippingPlaneBoundary(WVR_Eye eye, float* left, float* right, float* top, float* bottom)
{
	if (!isInitialized) {
		LOG_DP(Warning, "Not init, set default GetClippingPlaneBoundary");
		*left = -1;
		*right = 1;
		*top = 1;
		*bottom = -1;
	}
	else {
		LOG_FUNC();
		FUNC_PTR(GetClippingPlaneBoundary);
		FUNC_RUN_5(eye, left, right, top, bottom);
		FUNC_CHECK();
	}
}


WVR_Matrix4f_t WaveVRDirectPreview::GetTransformFromEyeToHead(WVR_Eye eye, WVR_NumDoF dof)
{
	if (!isInitialized) {
		return GetIdentityMatrix4f();
	}
	else {
		LOG_FUNC();
		FUNC_PTR(GetTransformFromEyeToHead);
		FUNC_RUN_2(eye, dof);
		FUNC_CHECK();
		return GetIdentityMatrix4f(); // failure
	}
}

bool WaveVRDirectPreview::GetRenderProps(WVR_RenderProps_t* props)
{
	if (!isInitialized) {
		LOG_DP(Warning, "Not init, GetRenderProps return false");
		return false;
	}
	else {
		LOG_FUNC();
		FUNC_PTR(GetRenderProps);
		FUNC_RUN_1(props);
		FUNC_CHECK();
		return false; // failure
	}
}

bool WaveVRDirectPreview::PollEventQueue(WVR_Event_t* event)
{
	// LOG_FUNC();  // too noisy
	FUNC_PTR(PollEventQueue);
	FUNC_RUN_1(event);
	FUNC_CHECK();
	return false; //failure
}

#if WITH_EDITOR
void WaveVRDirectPreview::SetPrintCallback(PrintLog callback){
	LOG_FUNC();
	static _WVR_SetPrintCallback funcPtr = nullptr;
	if (mDllHandle != nullptr && funcPtr == nullptr)
	{
		FString procName = "WVR_SetPrintCallback";
		funcPtr = (_WVR_SetPrintCallback) FPlatformProcess::GetDllExport(mDllHandle, *procName); \
	}
	FUNC_CHECK();
	FUNC_RUN_1(callback);
}

void WaveVRDirectPreview::DllLog(const char* msg)
{
	FString str(msg);
	LOGI(WVRDirectPreview, "%s", *str);
}

using namespace std::chrono;
milliseconds latestUpdateTime;
bool WaveVRDirectPreview::isTimeToUpdate() {
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	if ((ms -latestUpdateTime) >= milliseconds(1000 / mFPS)) {
		latestUpdateTime = ms;
		return true;
	}
	else {
		return false;
	}
}

DP_InitError WaveVRDirectPreview::ReadSettingsAndInit() {
	LOG_DP(Warning, "readSettingsAndInit Enter." );
	DP_InitError error = DP_InitError_None;

	auto VarConnectType = IConsoleManager::Get().FindConsoleVariable(TEXT("wvr.DirectPreview.ConnectType"));
	int ConnectType = VarConnectType->GetInt();

	auto VarDeviceWiFiIP = IConsoleManager::Get().FindConsoleVariable(TEXT("wvr.DirectPreview.DeviceWiFiIP"));
	FString DeviceWiFiIP = VarDeviceWiFiIP->GetString();
	char* mWiFiIP = TCHAR_TO_ANSI(*DeviceWiFiIP);

	auto VarEnablePreviewImage = IConsoleManager::Get().FindConsoleVariable(TEXT("wvr.DirectPreview.EnablePreviewImage"));
	int EnablePreviewImage = VarEnablePreviewImage->GetInt();
	mEnablePreviewImage = EnablePreviewImage;

	auto VarUpdateFrequency = IConsoleManager::Get().FindConsoleVariable(TEXT("wvr.DirectPreview.UpdateFrequency"));
	int UpdateFrequency = VarUpdateFrequency->GetInt();

	auto VarRegularlySaveImages = IConsoleManager::Get().FindConsoleVariable(TEXT("wvr.DirectPreview.RegularlySaveImages"));
	int RegularlySaveImages = VarRegularlySaveImages->GetInt();

	switch (UpdateFrequency)
	{
	case(0):
		mFPS = 1000;
		break;
	case(1):
		mFPS = 15;
		break;
	case(2):
		mFPS = 30;
		break;
	case(3):
		mFPS = 45;
		break;
	case(4):
		mFPS = 60;
		break;
	case(5):
		mFPS = 75;
		break;
	default:
		mFPS = 60;
		break;
	}
	LOGW(WVRDirectPreview, "readSettingsAndInit : ConntectType = %d , WifiIP = %s , EnablePreviewImage = %d , RegularlySaveImages = %d , FPS = %d",
		 ConnectType, PLATFORM_CHAR(*DeviceWiFiIP), EnablePreviewImage, RegularlySaveImages , mFPS);
	error = SimulatorInit((DP_ConnectType)ConnectType, mWiFiIP, EnablePreviewImage, false, RegularlySaveImages);

	return error;
}
#endif
