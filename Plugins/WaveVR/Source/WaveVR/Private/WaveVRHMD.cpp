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

// Engine
#include "RendererPrivate.h"
#include "ScenePrivate.h"
#include "SceneViewport.h"
#include "PostProcess/PostProcessHMD.h"
#include "BatteryStatusEvent.h"
#include "CtrlrSwipeEvent.h"
#include "IpdUpdateEvent.h"
#include "RequestResultObject.h"
#include "PipelineStateCache.h"
#include "Misc/ConfigCacheIni.h"
#include "XRThreadUtils.h"

// WaveVR
#include "commit_info.h"
#include "WaveVREventCommon.h"
#include "WaveVRSplash.h"
#include "WaveVRHMD.h"
#include "WaveVRRender.h"
#include "Platforms/WaveVRAPIWrapper.h"
#include "Platforms/Windows/WaveVRPlatformWindows.h"
#include "Platforms/Android/WaveVRPlatformAndroid.h"
#include "Platforms/Editor/WaveVRDirectPreview.h"
#include "IWaveVRPlugin.h"
#include "PoseManagerImp.h"
#include "WaveVRGestureEnums.h"
#include "WaveVRUtils.h"

DEFINE_LOG_CATEGORY_STATIC(WVRHMD, Display, All);

#if PLATFORM_ANDROID
#include "AndroidWindow.h"
#include "Android/AndroidEGL.h"
#endif

#ifdef DP_DEBUG
static int x1 = 0;
static int x2 = 0;
#endif

//---------------------------------------------------
// WaveVR Plugin Implementation
//---------------------------------------------------

class FWaveVRPlugin : public IWaveVRPlugin
{
	/** IHeadMountedDisplayModule implementation */
	virtual TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > CreateTrackingSystem() override;

	FString GetModuleKeyName() const {
		LOG_FUNC();
		return FString(TEXT("WaveVR"));
	}

public:
	FWaveVRPlugin() {
		LOG_FUNC();
#if PLATFORM_ANDROID
		LOGI(WVRHMD, "Commit Info : %s", WAVEVR_COMMIT_INFO);
#else // PLATFORM_WINDOWS || WITH_EDITOR
		FString str(WAVEVR_COMMIT_INFO);
		LOGI(WVRHMD, "Commit Info : %s", *str);
#endif
	}

	//TODO: Test whether it works or not if been removed
	virtual bool IsHMDConnected() override
	{
		LOG_FUNC();
		return true;
	}

	// This code should be here.  The FWaveVRPlugin must be the one who decide the platform, not decide in the wrapper.
	FWaveVRAPIWrapper * CreateWaveVRInstance()
	{
		FWaveVRAPIWrapper * instance = nullptr;
#if PLATFORM_ANDROID
		instance = new FWaveVRPlatformAndroid();
#elif WITH_EDITOR
		instance = FWaveVRHMD::DirectPreview = new WaveVRDirectPreview();
#elif PLATFORM_WINDOWS
		instance = new FWaveVRPlatformWindows();
#endif
		if (instance == nullptr)
			instance = new FWaveVRAPIWrapper();
		FWaveVRAPIWrapper::SetInstance(instance);

		return instance;
	}

	virtual void StartupModule() override
	{
		LOG_FUNC();
		IHeadMountedDisplayModule::StartupModule();
		// If use DirectPreview

		CreateWaveVRInstance();
		auto PlatFormContext = WVR();
		if (PlatFormContext != nullptr) {
			PlatFormContext->LoadLibraries();
		}
	}

	virtual void ShutdownModule() override
	{
		LOG_FUNC();
		IHeadMountedDisplayModule::ShutdownModule();
		auto PlatFormContext = WVR();
		if (PlatFormContext != nullptr) {
			PlatFormContext->UnLoadLibraries();
		}
	}
};

IMPLEMENT_MODULE( FWaveVRPlugin, WaveVR )

TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > FWaveVRPlugin::CreateTrackingSystem()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	TSharedPtr< FWaveVRHMD, ESPMode::ThreadSafe > WaveVRHMD = NewARSystem<FWaveVRHMD>();
	FWaveVRHMD::SetARSystem(WaveVRHMD);

	TSharedPtr< FWaveVRHMD::FWaveSceneViewExtension, ESPMode::ThreadSafe > SceneViewExtension = FSceneViewExtensions::NewExtension<FWaveVRHMD::FWaveSceneViewExtension>();
	WaveVRHMD->SetSceneViewExtension(SceneViewExtension);

	return WaveVRHMD;
}

//---------------------------------------------------
// WaveVR IHeadMountedDisplay Implementation
//---------------------------------------------------

FWaveVRHMD * FWaveVRHMD::Instance = nullptr;

WaveVRDirectPreview * FWaveVRHMD::DirectPreview = nullptr;

TSharedPtr<FARSystemBase, ESPMode::ThreadSafe> FWaveVRHMD::ArSystem = nullptr;

bool FWaveVRHMD::IsHMDEnabled() const
{
	LOG_FUNC();
	// This function will not be used in game, but editor.  If need preview, return true.
	return bHmdEnabled;
}

void FWaveVRHMD::EnableHMD(bool enable)
{
	LOG_FUNC();
	// Only the console command will call this function.  Therefore the bHmdEnabled is initially true.
	bHmdEnabled = enable;

	if (!bHmdEnabled)
	{
		EnableStereo(false);
	}
}

bool FWaveVRHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	LOG_FUNC();

	MonitorDesc.MonitorName = "";
	MonitorDesc.MonitorId = 0;
	MonitorDesc.DesktopX = 0;
	MonitorDesc.DesktopY = 0;
	MonitorDesc.ResolutionX = 0;
	MonitorDesc.ResolutionY = 0;

#if PLATFORM_ANDROID
	FPlatformRect Rect = FAndroidWindow::GetScreenRect();
	MonitorDesc.ResolutionX = Rect.Right - Rect.Left;
	MonitorDesc.ResolutionY = Rect.Bottom - Rect.Top;
	return true;
#elif PLATFORM_WINDOWS && !WITH_EDITOR
	MonitorDesc.ResolutionX = 1024;  // TODO Correct it.  This is a test code.
	MonitorDesc.ResolutionY = 1024;  // TODO Correct it.  This is a test code.
	return true;
#endif
	return false;
}

//TODO: re-check
void FWaveVRHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	LOG_FUNC();
	//Assigned zero means we use projection matrix.
	OutHFOVInDegrees = 0.0f;
	OutVFOVInDegrees = 0.0f;
}

bool FWaveVRHMD::DoesSupportPositionalTracking() const
{
	LOG_FUNC();
	return PoseMngr->GetSupportedNumOfDoF(WVR_DeviceType_HMD) == EWVR_DOF::DOF_6;
}

bool FWaveVRHMD::HasValidTrackingPosition()
{
	LOG_FUNC();
	//Support 6 Dof do not means set 6 Dof now. ref: PoseManagerImp::SetNumOfDoF
	return DoesSupportPositionalTracking() && PoseMngr->IsDevicePoseValid(WVR_DeviceType_HMD);
}

struct Wave_TrackedDevice {
	WVR_DeviceType DeviceType;
	EXRTrackedDeviceType EXRType;
};

Wave_TrackedDevice TrackedObjects[WVR_DEVICE_COUNT_LEVEL_1] = {
	{WVR_DeviceType_HMD              /*1*/, EXRTrackedDeviceType::HeadMountedDisplay},
	{WVR_DeviceType_Controller_Right /*2*/, EXRTrackedDeviceType::Controller},
	{WVR_DeviceType_Controller_Left  /*3*/, EXRTrackedDeviceType::Controller},
};

bool FWaveVRHMD::EnumerateTrackedDevices(TArray<int32>& OutDevices, EXRTrackedDeviceType Type /*EXRTrackedDeviceType::Any*/)
{
	LOG_FUNC()
	for (auto TrackedDevice: TrackedObjects){
		if (TrackedDevice.EXRType == Type || Type == EXRTrackedDeviceType::Any)
			OutDevices.Add(static_cast<int32>(TrackedDevice.DeviceType));
	}
	return true;
}

#if (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG) && !WITH_EDITOR

#define WVR_SCOPED_NAMED_EVENT(name, color) SCOPED_NAMED_EVENT(name, color)

#else

#define WVR_SCOPED_NAMED_EVENT(name, color)

#endif



// IXRTrackingSystem: This method must be callable both on the render thread and the game thread.
bool FWaveVRHMD::GetCurrentPose(int32 DeviceId, FQuat& OutOrientation, FVector& OutPosition) {
	LOG_FUNC();
	WVR_SCOPED_NAMED_EVENT(GetCurrentPose, FColor::Orange);

	//if (PLATFORM_WINDOWS) // TODO Correct it.  This is a test code.
	//{
	//	OutOrientation = FQuat::Identity;
	//	OutPosition = FVector::ZeroVector;
	//	return true;
	//}

#if WITH_EDITOR
	if (GIsEditor && !WaveVRDirectPreview::IsDirectPreview()) return false;
#endif

	WVR_DeviceType ID = static_cast<WVR_DeviceType>(DeviceId);
	if (DeviceId == IXRTrackingSystem::HMDDeviceId) { ID = WVR_DeviceType_HMD; } // 0 is the default (first) HMD which UE assigned.

	if (IsInGameThread()) {
		OutOrientation = FrameData->gameOrientation;
		OutPosition = FrameData->gamePosition;
		return true;
	} else if (IsInRenderingThread()) {
		bool ret = false;
		if (FrameDataRT->bSupportLateUpdate && FrameDataRT->bNeedLateUpdateInRT) {
			FrameDataRT->bNeedLateUpdateInRT = false;
			PoseMngr->LateUpdate_RenderThread(FrameDataRT);
		}
		OutOrientation = FrameDataRT->gameOrientation;
		OutPosition = FrameDataRT->gamePosition;
		return true;
	}

	return false;
}

bool FWaveVRHMD::GetRelativeEyePose(int32 DeviceId, EStereoscopicPass Eye, FQuat& OutOrientation, FVector& OutPosition)
{
	LOG_FUNC();
	OutOrientation = FQuat::Identity;
	OutPosition = FVector::ZeroVector;
	//if (PLATFORM_WINDOWS) // TODO Correct it.  This is a test code.
	//	return true;

	if (DeviceId == IXRTrackingSystem::HMDDeviceId && (Eye == eSSP_LEFT_EYE || Eye == eSSP_RIGHT_EYE)) {
		WVR_Eye hmdEye = (Eye == eSSP_LEFT_EYE) ? WVR_Eye_Left : WVR_Eye_Right;
		WVR_Matrix4f_t headFromEye = WVR()->GetTransformFromEyeToHead(hmdEye/*, WVR_NumDoF_6DoF*/);  // TODO need input DoF

		OutPosition = FVector(-headFromEye.m[2][3], headFromEye.m[0][3], headFromEye.m[1][3]) * GetWorldToMetersScale();

		FQuat Orientation(wvr::utils::ToFMatrix(headFromEye));
		OutOrientation.X = -Orientation.Z;
		OutOrientation.Y = Orientation.X;
		OutOrientation.Z = Orientation.Y;
		OutOrientation.W = -Orientation.W;
		//LOGI(WVRHMD, "GetRelativeEyePose(%d, %d, Quat(%f, %f, %f, %f), Pos(%f, %f, %f))", DeviceId, Eye, OutOrientation.X, OutOrientation.Y, OutOrientation.Z, OutOrientation.W, OutPosition.X, OutPosition.Y, OutPosition.Z);
		return true;
	} else {
		return false;
	}
	//return FXRTrackingSystemBase::GetRelativeEyePose(DeviceId, Eye, OutOrientation, OutPosition);
}

void FWaveVRHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
	LOG_FUNC();
	//Do not allow user to set IPD
	LOGI(WVRHMD, "SetInterpupillaryDistance %.5f, but nothing happened.", NewInterpupillaryDistance);
}

float FWaveVRHMD::GetInterpupillaryDistance() const
{
	LOG_FUNC();
	return CurrentIPD;
}

bool FWaveVRHMD::GetHMDDistortionEnabled(EShadingPath ShadingPath) const {
	LOG_FUNC();
	return bUseUnrealDistortion;
}

void FWaveVRHMD::SetPixelDensity(const float NewDensity) {
	LOG_FUNC();
	/* TODO Handle it when density larger than one. */
	if (NewDensity < 0.1f || NewDensity > 1.0f)
		PixelDensity = FMath::Clamp(NewDensity, 0.1f, 1.0f);
	else
		PixelDensity = NewDensity;
}

FIntPoint FWaveVRHMD::GetIdealRenderTargetSize() const {
	LOG_FUNC();
	FIntPoint IdealRenderTargetSize;
	uint32 width = mRender.GetSingleEyePixelWidth(), height = mRender.GetSingleEyePixelHeight();
	IdealRenderTargetSize.X = mRender.GetMultiView() ? width : width * 2;
	IdealRenderTargetSize.Y = height;
	return IdealRenderTargetSize;
}

bool FWaveVRHMD::IsChromaAbCorrectionEnabled() const
{
	LOG_FUNC();
	return true;
}

bool FWaveVRHMD::IsHeadTrackingAllowed() const
{
	LOG_FUNC();
#if WITH_EDITOR
	if (GIsEditor)
	{
		// @todo vreditor: We need to do a pass over VREditor code and make sure we are handling the VR modes correctly.  HeadTracking can be enabled without Stereo3D, for example
		UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine);
		//LOGI(WVRHMD,"IsHeadTrackingAllowed !EdEngine(%u) EdEngine->IsHMDTrackingAllowed()(%u) IsStereoEnabled()(%u)", EdEngine, EdEngine->IsHMDTrackingAllowed(), IsStereoEnabled());
		return (!EdEngine || EdEngine->IsHMDTrackingAllowed()) && IsStereoEnabled();
	}
#endif // WITH_EDITOR
	return true;
}

void FWaveVRHMD::OnBeginPlay(FWorldContext& InWorldContext)
{
	LOG_FUNC();
	FirstGameFrame = true;

	WaveVRDirectPreviewSettings::SettingsInfo();

	if (WaveVRDirectPreview::IsVRPreview() && !bSIM_Available) //OnBeginPlay will continuously execute twice
	{
		if (DirectPreview != nullptr && DirectPreview->HookVRPreview()) {
			LOGD(WVRHMD, "DirectPreview device is hooked.");
			bSIM_Available = true;
			ResetProjectionMats();

			uint32 width = 1024, height = 1024;
			WVR()->GetRenderTargetSize(&width, &height);
			mRender.SetSingleEyePixelSize(width, height);
			mRender.SetTextureFormat(PF_R8G8B8A8);
			mRender.Apply();

			setDeviceSupportedMaxFPS();

		} else {
			LOGW(WVRHMD, "DirectPreview hook failed!");
		}
	}
}

void FWaveVRHMD::OnEndPlay(FWorldContext& InWorldContext)
{
	LOG_FUNC();
	EnableStereo(false);
	if (WaveVRDirectPreview::IsVRPreview())
	{
		// Close it if available
		if (bSIM_Available) {
			WVR()->Quit();
			bSIM_Available = false;
		}
	}
}

void FWaveVRHMD::NextFrameData() {
	// swap data
	OldFrameData = FrameData;
	FrameData = FFrameData::NewInstance();

	// If position is invalidate, we need the old pose.  Use latest pose in RT.
	FFrameData::Copy(OldFrameDataRT, FrameData);

	FrameData->bSupportLateUpdate = FrameData->bNeedLateUpdateInRT = lateUpdateConfig.bEnabled;
	FrameData->bDoUpdateInGT = lateUpdateConfig.bDoUpdateInGT;
	FrameData->predictTimeInGT = lateUpdateConfig.predictTimeInGT;
	FrameData->frameNumber = GFrameNumber;
	FrameData->meterToWorldUnit = GetWorldToMetersScale();  // For example, 1 meter multiply 100 to convert to world units.
	FrameData->baseOrientation = BaseOrientation;
	FrameData->basePosition = CompensationOffset;
	FrameData->Origin = PoseMngr->GetTrackingOriginModelInternal();
}

bool FWaveVRHMD::OnStartGameFrame(FWorldContext& WorldContext)
{
	LOG_FUNC();
	WVR_SCOPED_NAMED_EVENT(OnStartGameFrame, FColor::Purple);

#if WITH_EDITOR
#ifdef DP_DEBUG
	LOGW(WVRHMD, "FlushRenderingCommands");
#endif
	FlushRenderingCommands();
#endif

	// TODO Correct it.  This may be needed in windows
	if (!GIsEditor) {
		SetInputRequest(WVR_DeviceType::WVR_DeviceType_HMD, InputAttributes_HMD, InputAttributes_HMD_Count);
		SetInputRequest(WVR_DeviceType::WVR_DeviceType_Controller_Right, InputAttributes_Controller, InputAttributes_Controller_Count);
		SetInputRequest(WVR_DeviceType::WVR_DeviceType_Controller_Left, InputAttributes_Controller, InputAttributes_Controller_Count);
	}

#if WITH_EDITOR
	if (WorldContext.WorldType == EWorldType::PIE && (WaveVRDirectPreview::IsVRPreview() && IsDirectPreview())) {
#ifdef DP_DEBUG
		LOGW(WVRHMD, "OnStartGameFrame %d", x1++);
#endif
#else
	if (mRender.IsInitialized()) {
#endif
		NextFrameData();
		PoseMngr->UpdatePoses(FrameData);
	}

	RefreshTrackingToWorldTransform(WorldContext);
	pollEvent();
	checkSystemFocus();

	if (GNearClippingPlane != NearClippingPlane) {
		bNeedResetProjectionMatrix = true;
		NearClippingPlane = GNearClippingPlane;
	}
	if (bNeedResetProjectionMatrix)
		ResetProjectionMats();

	SceneStatusInfo(FirstGameFrame);
	FirstGameFrame = false;

	CachedTrackingToWorld = ComputeTrackingToWorldTransform(WorldContext);

	mWaveAR->UpdateData();

#if WITH_EDITOR
	WVR()->Tick();
#endif

#if (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG)
	if (SimulatedLoadingGameThread > 0 && SimulatedLoadingGameThread <= 100000)
	{
		WVR_SCOPED_NAMED_EVENT(GTSleep, FColor::Purple);
		FPlatformProcess::Sleep(SimulatedLoadingGameThread / 1000000.0f);
	}
#endif

	return true;
}

bool FWaveVRHMD::OnEndGameFrame(FWorldContext& WorldContext)
{
	WVR_SCOPED_NAMED_EVENT(OnEndGameFrame, FColor::Purple);

	LOG_FUNC();
	//LOGD(WVRHMD, "OnEndGameFrame");

	FrameDataPtr FrameDataCopy = FFrameData::NewInstance();
	FFrameData::Copy(FrameData, FrameDataCopy);

	// Send a copy of FrameData to render thread.
	ExecuteOnRenderThread_DoNotWait([this, FrameDataCopy](FRHICommandListImmediate& RHICmdList)
	{
		OldFrameDataRT = FrameDataRT;
		FrameDataRT = FrameDataCopy;
	}
	);

	return false;
}


void FWaveVRHMD::OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	WVR_SCOPED_NAMED_EVENT(OnBeginRendering, FColor::Purple);

	LOG_FUNC();
	// TODO We should let simulator run the mRender to test windows path.
	if (GIsEditor) return;

#if (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG)
	if (SimulatedLoadingRenderThread > 0 && SimulatedLoadingRenderThread <= 100000)
	{
		WVR_SCOPED_NAMED_EVENT(RTSleep, FColor::Purple);
		FPlatformProcess::Sleep(SimulatedLoadingRenderThread / 1000000.0f);
	}
#endif

	mRender.OnBeginRendering_RenderThread(RHICmdList, ViewFamily);
}

void FWaveVRHMD::OnBeginRendering_GameThread()
{
	WVR_SCOPED_NAMED_EVENT(OnBeginRendering, FColor::Purple);
	LOG_FUNC();
}

bool FWaveVRHMD::DoesSupportLateUpdate() const {
	LOG_FUNC();
	//LOGD(WVRHMD, "LateUpdate=%d, DoUpdateInGT=%d, predict=%f",
	//	lateUpdateConfig.bEnabled, lateUpdateConfig.bDoUpdateInGT, lateUpdateConfig.predictTimeInGT);
	return lateUpdateConfig.bEnabled;
}

void FWaveVRHMD::EnableLateUpdate(bool enable, bool doUpdateInGT, float predictTimeInGT) {
	predictTimeInGT = FMath::Clamp<float>(predictTimeInGT, 0, 100.0f);
	LOGD(WVRHMD, "EnableLateUpdate(%d, %d, %f)", (int) enable, (int)doUpdateInGT, predictTimeInGT);
	lateUpdateConfig.bEnabled = enable;
	lateUpdateConfig.bDoUpdateInGT = doUpdateInGT;
	lateUpdateConfig.predictTimeInGT = predictTimeInGT;

	// Trigger RenderMask to disable or enable
	UIpdUpdateEvent::onIpdUpdateNative.Broadcast();
}

void FWaveVRHMD::OnLateUpdateApplied_RenderThread(const FTransform& NewRelativeTransform)
{
	WVR_SCOPED_NAMED_EVENT(OnLateUpdateApplied, FColor::Purple);
	LOG_FUNC();
}


void FWaveVRHMD::SetTrackingOrigin(EHMDTrackingOrigin::Type InOrigin)
{
	LOG_FUNC();
	PoseMngr->SetTrackingOriginPoses(InOrigin);
}


EHMDTrackingOrigin::Type FWaveVRHMD::GetTrackingOrigin()
{
	LOG_FUNC();
	return PoseMngr->GetTrackingOriginPoses();
}

void FWaveVRHMD::pollEvent() {
	LOG_FUNC();
	bool ret = false;
	do
	{
		WVR_Event_t vrevent;
		ret = WVR()->PollEventQueue(&vrevent);
		if (ret)
			processVREvent(vrevent);
	} while (ret);
}

void FWaveVRHMD::processVREvent(WVR_Event_t vrEvent)
{
	LOG_FUNC();
	WVR_DeviceType _dt = vrEvent.device.deviceType;
	WVR_InputId _btn = vrEvent.input.inputId;
	switch (vrEvent.common.type)
	{
	case WVR_EventType_DeviceConnected:
		LOGD(WVRHMD, "processVREvent() WVR_EventType_DeviceConnected device %d", (uint8)_dt);
		if (_dt == WVR_DeviceType::WVR_DeviceType_HMD) {
			bIsHmdConnected = true;
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right) {
			bIsRightDeviceConnected = true;
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left) {
			bIsLeftDeviceConnected = true;
		}
		UWaveVREventCommon::OnConnectionChangeNative.Broadcast((uint8)_dt, true);
		break;
	case WVR_EventType_DeviceDisconnected:
		LOGD(WVRHMD, "processVREvent() WVR_EventType_DeviceDisconnected device %d", (uint8)_dt);
		if (_dt == WVR_DeviceType::WVR_DeviceType_HMD) {
			bIsHmdConnected = false;
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right) {
			bIsRightDeviceConnected = false;
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left) {
			bIsLeftDeviceConnected = false;
		}
		UWaveVREventCommon::OnConnectionChangeNative.Broadcast((uint8)_dt, false);
		break;
	case WVR_EventType_BatteryStatusUpdate:
		LOGD(WVRHMD, "WVR_EventType: WVR_EventType battery status updated");
		UBatteryStatusEvent::onBatteryStatusUpdateNative.Broadcast();
		break;
	case WVR_EventType_IpdChanged:
		WVR_RenderProps props;
		if (WVR()->GetRenderProps(&props)) {
			float ipd = props.ipdMeter;
			LOGI(WVRHMD, "WVR_EventType: WVR_EventType IPD updated (%f)", ipd);
			CurrentIPD = ipd;
		}
		else {
			LOGD(WVRHMD, "Get render properties error! Not success!");
		}
		ResetProjectionMats();
		UIpdUpdateEvent::onIpdUpdateNative.Broadcast();
		break;
	case WVR_EventType_LeftToRightSwipe:
		LOGD(WVRHMD, "WVR_EventType: touch pad swipe (left to right)");
		UCtrlrSwipeEvent::onCtrlrSwipeLtoRUpdateNative.Broadcast();
		break;
	case WVR_EventType_RightToLeftSwipe:
		LOGD(WVRHMD, "WVR_EventType: touch pad swipe (right to left)");
		UCtrlrSwipeEvent::onCtrlrSwipeRtoLUpdateNative.Broadcast();
		break;
	case WVR_EventType_UpToDownSwipe:
		LOGD(WVRHMD, "WVR_EventType: touch pad swipe (up to down)");
		UCtrlrSwipeEvent::onCtrlrSwipeUtoDUpdateNative.Broadcast();
		break;
	case WVR_EventType_DownToUpSwipe:
		LOGD(WVRHMD, "WVR_EventType: touch pad swipe (down to up)");
		UCtrlrSwipeEvent::onCtrlrSwipeDtoUUpdateNative.Broadcast();
		break;
	case WVR_EventType_RecenterSuccess:
		LOGD(WVRHMD, "WVR_EventType: WVR_EventType HMD Recenter");
		if (!CompensationOffset.IsZero() || !BaseOrientation.IsIdentity()) {
			FrameData->basePosition = CompensationOffset = FVector::ZeroVector;
			ResetOrientation(Current_Yaw);
		}
		break;
	case WVR_EventType_DeviceRoleChanged:
		LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_DeviceRoleChanged");
		UWaveVREventCommon::OnControllerRoleChangeNative.Broadcast();
		isInputRequested_Hmd = false;
		isInputRequested_Right = false;
		isInputRequested_Left = false;
		for (int _i = 0; _i < (int)WVR_InputId_Max; _i++)
		{
			usableButtons_right[_i] = WVR_InputId_Max;
			usableButtons_left[_i] = WVR_InputId_Max;
			usableButtons_hmd[_i] = WVR_InputId_Max;
		}

		bIsHmdConnected = PoseMngr->IsDeviceConnected(WVR_DeviceType::WVR_DeviceType_HMD);
		UWaveVREventCommon::OnConnectionChangeNative.Broadcast((uint8)WVR_DeviceType::WVR_DeviceType_HMD, bIsHmdConnected);
		bIsRightDeviceConnected = PoseMngr->IsDeviceConnected(WVR_DeviceType::WVR_DeviceType_Controller_Right);
		UWaveVREventCommon::OnConnectionChangeNative.Broadcast((uint8)WVR_DeviceType::WVR_DeviceType_Controller_Right, bIsRightDeviceConnected);
		bIsLeftDeviceConnected = PoseMngr->IsDeviceConnected(WVR_DeviceType::WVR_DeviceType_Controller_Left);
		UWaveVREventCommon::OnConnectionChangeNative.Broadcast((uint8)WVR_DeviceType::WVR_DeviceType_Controller_Left, bIsLeftDeviceConnected);
		break;
	case WVR_EventType_TrackingModeChanged:
		LOGD(WVRHMD, "WVR_EventType: WVR_EventType_TrackingModeChanged");
		UWaveVREventCommon::OnTrackingModeChangeNative.Broadcast();
		break;
		/* ------------------- Button State begin ---------------- */
	case WVR_EventType_ButtonPressed:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_ButtonPressed broadcast to OnAllEventPressNative_Right, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventPressNative_Right.Broadcast((uint8)_btn, true);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_ButtonPressed broadcast to OnAllEventPressNative_Left, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventPressNative_Left.Broadcast((uint8)_btn, true);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_HMD)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_ButtonPressed broadcast to OnAllEventPressNative_HMD, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventPressNative_HMD.Broadcast((uint8)_btn, true);
		}
		break;
	case WVR_EventType_ButtonUnpressed:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_ButtonUnpressed broadcast to OnAllEventPressNative_Right, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventPressNative_Right.Broadcast((uint8)_btn, false);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_ButtonUnpressed broadcast to OnAllEventPressNative_Left, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventPressNative_Left.Broadcast((uint8)_btn, false);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_HMD)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_ButtonUnpressed broadcast to OnAllEventPressNative_HMD, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventPressNative_HMD.Broadcast((uint8)_btn, false);
		}
		break;
	case WVR_EventType_TouchTapped:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_TouchTapped broadcast to OnAllEventTouchNative_Right, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventTouchNative_Right.Broadcast((uint8)_btn, true);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_TouchTapped broadcast to OnAllEventTouchNative_Left, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventTouchNative_Left.Broadcast((uint8)_btn, true);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_HMD)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_TouchTapped broadcast to OnAllEventTouchNative_HMD, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventTouchNative_HMD.Broadcast((uint8)_btn, true);
		}
		break;
	case WVR_EventType_TouchUntapped:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_TouchUntapped broadcast to OnAllEventTouchNative_Right, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventTouchNative_Right.Broadcast((uint8)_btn, false);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_TouchUntapped broadcast to OnAllEventTouchNative_Left, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventTouchNative_Left.Broadcast((uint8)_btn, false);
		}
		else if (_dt == WVR_DeviceType::WVR_DeviceType_HMD)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_TouchUntapped broadcast to OnAllEventTouchNative_HMD, _btn %d", (uint8)_btn);
			UWaveVREventCommon::OnAllEventTouchNative_HMD.Broadcast((uint8)_btn, false);
		}
		break;
		/* ------------------- Button State end ---------------- */
		/* ------------------- Gesture begin ------------------- */
	case WVR_EventType_HandGesture_Abnormal:
	case WVR_EventType_HandTracking_Abnormal:
		gestureEventCurr = vrEvent.common.type;
		break;
	/*case WVR_EventType_HandMotion_SwipingRight:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingRight broadcast to OnDynamicGestureNative_Left.");
			UWaveVREventCommon::OnDynamicGestureNative_Left.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingRight);
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingRight broadcast to OnDynamicGestureNative_Right.");
			UWaveVREventCommon::OnDynamicGestureNative_Right.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingRight);
		}
		break;
	case WVR_EventType_HandMotion_SwipingLeft:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingLeft broadcast to OnDynamicGestureNative_Left.");
			UWaveVREventCommon::OnDynamicGestureNative_Left.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingLeft);
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingLeft broadcast to OnDynamicGestureNative_Right.");
			UWaveVREventCommon::OnDynamicGestureNative_Right.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingLeft);
		}
		break;
	case WVR_EventType_HandMotion_SwipingUp:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingUp broadcast to OnDynamicGestureNative_Left.");
			UWaveVREventCommon::OnDynamicGestureNative_Left.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingUp);
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingUp broadcast to OnDynamicGestureNative_Right.");
			UWaveVREventCommon::OnDynamicGestureNative_Right.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingUp);
		}
		break;
	case WVR_EventType_HandMotion_SwipingDown:
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Left)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingDown broadcast to OnDynamicGestureNative_Left.");
			UWaveVREventCommon::OnDynamicGestureNative_Left.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingDown);
		}
		if (_dt == WVR_DeviceType::WVR_DeviceType_Controller_Right)
		{
			LOGD(WVRHMD, "WaveVR processVREvent() WVR_EventType_HandMotion_SwipingDown broadcast to OnDynamicGestureNative_Right.");
			UWaveVREventCommon::OnDynamicGestureNative_Right.Broadcast((uint8)EWaveVRDynamicGesture::TYPE_SwipingDown);
		}
		break;*/
		/* ------------------- Gesture end ------------------- */
	default:
		break;
	}

	UWaveVREventCommon::OnAllEventNative.Broadcast((int32)(vrEvent.common.type));
}

void FWaveVRHMD::ResetOrientationAndPosition(float yaw) {
	LOG_FUNC();
	ResetOrientation(yaw);
	ResetPosition();
}

void FWaveVRHMD::ResetOrientation(float Yaw) {
	LOG_FUNC();
	FRotator NewBaseRotation(0, Yaw, 0);
	Current_Yaw = Yaw;
	FrameData->baseOrientation = BaseOrientation = NewBaseRotation.Quaternion();
	PoseMngr->UpdateDevice(FrameData);
}

void FWaveVRHMD::ResetPosition() {
	LOG_FUNC();
	FrameData->basePosition = CompensationOffset = PoseMngr->GetDevice(WVR_DeviceType_HMD)->position;
	PoseMngr->UpdateDevice(FrameData);
}

void FWaveVRHMD::SetBaseRotation(const FRotator& BaseRot) {
	LOG_FUNC();
	BaseOrientation = BaseRot.Quaternion();
}

FRotator FWaveVRHMD::GetBaseRotation() const {
	LOG_FUNC();
	return BaseOrientation.Rotator();
}

void FWaveVRHMD::SetBaseOrientation(const FQuat& BaseOrient) {
	LOG_FUNC();
	BaseOrientation = BaseOrient;
}

FQuat FWaveVRHMD::GetBaseOrientation() const {
	LOG_FUNC();
	return BaseOrientation;
}

FVector FWaveVRHMD::GetCompensationOffset() const {
	LOG_FUNC();
	return CompensationOffset;
}

void FWaveVRHMD::checkSystemFocus() {
	// TODO IsInputFocusCapuredBySystem() will block!
	bFocusCapturedBySystem = WVR()->IsInputFocusCapturedBySystem();
}

bool FWaveVRHMD::isInputRequested(WVR_DeviceType device) const
{
	bool _ret = false;
	switch (device)
	{
	case WVR_DeviceType::WVR_DeviceType_HMD:
		_ret = isInputRequested_Hmd;
		break;
	case WVR_DeviceType::WVR_DeviceType_Controller_Right:
		_ret = isInputRequested_Right;
		break;
	case WVR_DeviceType::WVR_DeviceType_Controller_Left:
		_ret = isInputRequested_Left;
		break;
	default:
		break;
	}
	return _ret;
}

bool FWaveVRHMD::IsButtonAvailable(WVR_DeviceType device, WVR_InputId button)
{
	if (button == WVR_InputId_Max) return false;

	for (int i = 0; i < (int)WVR_InputId_Max; i++)
	{
		switch (device)
		{
		case WVR_DeviceType_HMD:
			if (usableButtons_hmd[i] == button)
				return true;
			break;
		case WVR_DeviceType_Controller_Right:
			if (usableButtons_right[i] == button)
				return true;
			break;
		case WVR_DeviceType_Controller_Left:
			if (usableButtons_left[i] == button)
				return true;
			break;
		default:
			break;
		}
	}

	return false;
}

void FWaveVRHMD::SetInputRequest(WVR_DeviceType device, const WVR_InputAttribute_t* inputAttributes, uint32_t size) {
	WVR_InputMappingPair _table[(int)WVR_InputId_Max];
	uint32_t _count = 0;
	switch (device)
	{
	case WVR_DeviceType::WVR_DeviceType_HMD:
		if (!isInputRequested_Hmd)
		{
			LOGV(WVRHMD, "WaveVR SetInputRequest() of HMD");
			isInputRequested_Hmd = WVR()->SetInputRequest(device, inputAttributes, size);
			if (isInputRequested_Hmd)
			{
				_count = WVR()->GetInputMappingTable(device, _table, (uint32_t)WVR_InputId_Max);
				for (int _i = 0; _i < (int)_count; _i++)
				{
					if (_table[_i].source.capability != 0)
					{
						LOGD(WVRHMD, "SetInputRequest() HMD source %d, dest %d", (uint8)_table[_i].source.id, (uint8)_table[_i].destination.id);
						usableButtons_hmd[_i] = _table[_i].destination.id;
					}
				}
			}
		}
		break;
	case WVR_DeviceType::WVR_DeviceType_Controller_Right:
		if (!isInputRequested_Right)
		{
			LOGV(WVRHMD, "WaveVR SetInputRequest() of Right");
			isInputRequested_Right = WVR()->SetInputRequest(device, inputAttributes, size);
			if (isInputRequested_Right)
			{
				_count = WVR()->GetInputMappingTable(device, _table, (uint32_t)WVR_InputId_Max);
				for (int _i = 0; _i < (int)_count; _i++)
				{
					if (_table[_i].source.capability != 0)
					{
						LOGD(WVRHMD, "SetInputRequest() Right source %d, dest %d", (uint8)_table[_i].source.id, (uint8)_table[_i].destination.id);
						usableButtons_right[_i] = _table[_i].destination.id;
					}
				}
			}
		}
		break;
	case WVR_DeviceType::WVR_DeviceType_Controller_Left:
		if (!isInputRequested_Left)
		{
			LOGV(WVRHMD, "WaveVR SetInputRequest() of Left");
			isInputRequested_Left = WVR()->SetInputRequest(device, inputAttributes, size);
			if (isInputRequested_Left)
			{
				_count = WVR()->GetInputMappingTable(device, _table, (uint32_t)WVR_InputId_Max);
				for (int _i = 0; _i < (int)_count; _i++)
				{
					if (_table[_i].source.capability != 0)
					{
						LOGD(WVRHMD, "SetInputRequest() Left source %d, dest %d", (uint8)_table[_i].source.id, (uint8)_table[_i].destination.id);
						usableButtons_left[_i] = _table[_i].destination.id;
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

bool FWaveVRHMD::IsFocusCapturedBySystem() const {
	return bFocusCapturedBySystem;
}

bool FWaveVRHMD::IsDeviceConnected(WVR_DeviceType device) const {
	if (device == WVR_DeviceType::WVR_DeviceType_HMD) {
		return bIsHmdConnected;
	}
	if (device == WVR_DeviceType::WVR_DeviceType_Controller_Right) {
		return bIsRightDeviceConnected;
	}
	if (device == WVR_DeviceType::WVR_DeviceType_Controller_Left) {
		return bIsLeftDeviceConnected;
	}

	return false;
}

bool FWaveVRHMD::IsStereoEnabledInternal() const
{
	// Internal function will not use the LOG_FUNC()
	return bStereoEnabled && bHmdEnabled;
}

bool FWaveVRHMD::IsStereoEnabled() const
{
	LOG_FUNC();
	return bStereoEnabled && bHmdEnabled;
}

float FWaveVRHMD::GetWorldToMetersScale() const
{
	LOG_FUNC();
	if (IsInGameThread() && GWorld != nullptr)
	{
		// For example, One world unit need multiply 100 to become 1 meter.
		float wtm = GWorld->GetWorldSettings()->WorldToMeters;
		//LOGI(WVRHMD, "GWorld->GetWorldSettings()->WorldToMeters = %f", wtm);
		return wtm;
	}
	return 100.0f;
}

bool FWaveVRHMD::EnableStereo(bool bStereo)
{
	LOG_FUNC();
	// Only Enabled if the develop check the 'Start in VR' from project settings.  Therefore the bStereoEnabled could be initiallized true.
	LOGI(WVRHMD, "EnableStereo = %d", bStereo);

	bStereoEnabled = bStereo;
	return bStereoEnabled;
}

void FWaveVRHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
	LOG_FUNC();
#if WITH_EDITOR
	if (GIsEditor) {
		// We should not use these input values.  How to get the screen size?
		SizeX = SizeX / 2;
		SizeY = SizeY;

		uint32 newWidth = FMath::CeilToInt(SizeX * PixelDensity);
		uint32 newHeight = FMath::CeilToInt(SizeY * PixelDensity);

		// Put in center
		X = FMath::CeilToInt((SizeX - newWidth) / 2) + (StereoPass == eSSP_RIGHT_EYE ? SizeX : 0);
		Y = FMath::CeilToInt((SizeY - newHeight) / 2);

		SizeX = newWidth;
		SizeY = newHeight;

		return;
	}
#endif

	if (!mRender.IsCustomPresentSet()) {
		LOGD(WVRHMD, "mRender is not ready for VR.");
		return;
	}

	uint32 width = mRender.GetSingleEyePixelWidth(), height = mRender.GetSingleEyePixelHeight();

	uint32 newWidth = FMath::CeilToInt(width * PixelDensity);
	uint32 newHeight = FMath::CeilToInt(height * PixelDensity);

	if (GSupportsMobileMultiView) {
		SizeX = newWidth;
		SizeY = newHeight;

		// Put in center
		X = FMath::CeilToInt((width - newWidth) / 2);
		Y = FMath::CeilToInt((height - newHeight) / 2);
		return;
	} else {
		SizeX = newWidth;
		SizeY = newHeight;

		// Put in center
		X = FMath::CeilToInt((width - newWidth) / 2) + (StereoPass == eSSP_RIGHT_EYE ? width : 0);
		Y = FMath::CeilToInt((height - newHeight) / 2);
	}
	//LOGI(WVRHMD, "AdjustViewRect eye %d (%d, %d, %d, %d)", (int)StereoPass, X, Y, SizeX, SizeY);
}

void FWaveVRHMD::SetFinalViewRect(const enum EStereoscopicPass StereoPass, const FIntRect& FinalViewRect) {
	LOG_FUNC();
	//LOGI(WVRHMD, "SetFinalViewRect StereoPass(%d) FinalViewRect MinX(%d),MaxX(%d), MinY(%d),MaxY(%d)", (int)StereoPass, FinalViewRect.Min.X, FinalViewRect.Max.X, FinalViewRect.Min.Y, FinalViewRect.Max.Y);
}

// Will call to GetRelativeEyePose
void FWaveVRHMD::CalculateStereoViewOffset(const EStereoscopicPass StereoPassType, FRotator& ViewRotation, const float WorldToMeters, FVector& ViewLocation)
{
	LOG_FUNC();
	GetXRCamera()->CalculateStereoCameraOffset(StereoPassType, ViewRotation, ViewLocation);
}

FMatrix FWaveVRHMD::GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType) const
{
	LOG_FUNC();
	if (!IsStereoEnabledInternal())
		return FMatrix::Identity;

	if (StereoPassType == eSSP_LEFT_EYE) {
		return LeftProjection;
	} else if (StereoPassType == eSSP_RIGHT_EYE) {
		return RightProjection;
	} else if (StereoPassType == eSSP_MONOSCOPIC_EYE) {
		return CenterProjection;
	}

	return FMatrix::Identity;
}

void FWaveVRHMD::InitCanvasFromView(FSceneView* InView, UCanvas* Canvas)
{
	LOG_FUNC();
}

FWaveVRHMD::FWaveVRHMD()
	: bUseUnrealDistortion(GIsEditor)

	, isInputRequested_Hmd(false)
	, isInputRequested_Right(false)
	, isInputRequested_Left(false)

	, gestureEventCurr(WVR_EventType::WVR_EventType_HandGesture_Changed)
	, handGestureEnabled(false)
	, handTrackingEnabled(false)

	, bResumed(false)
	, supportedFeatures(0)

	, DistortionPointsX(40)
	, DistortionPointsY(40)
	, NumVerts(0)
	, NumTris(0)
	, NumIndices(0)
	, DistortionMeshIndices(nullptr)
	, DistortionMeshVerticesLeftEye(nullptr)
	, DistortionMeshVerticesRightEye(nullptr)

	, bIsHmdConnected(false)
	, bIsRightDeviceConnected(false)
	, bIsLeftDeviceConnected(false)
	, bHmdEnabled(true)
	, bStereoEnabled(false)
	, CurrentIPD(0.064f)

	, NearClippingPlane(10.0f)
	, FarClippingPlane(0.0f)
	, bNeedResetProjectionMatrix(true)

	, mRender(this)
	, PoseMngr(PoseManagerImp::GetInstance())
	, mWaveAR(FWaveAR::GetInstance())

	, DeltaControlRotation(FRotator::ZeroRotator)
	, DeltaControlOrientation(FQuat::Identity)

	, BaseOrientation(FQuat::Identity)
	, CompensationOffset(FVector::ZeroVector)
	, Current_Yaw(0.f)

	, PixelDensity(1.f)
	, bFocusCapturedBySystem(false)
	, bIsInAppRecenter(false)
	, bSIM_Available(false)
	, FirstGameFrame(true)
	, bAdaptiveQuality(false)

	, lateUpdateConfig()

	, SimulatedLoadingGameThread(0)
	, SimulatedLoadingRenderThread(0)
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);

	Startup();

#define LOCAL_EOL ", "  // " \n"
	LOGI(WVRHMD, "Initialized variable:" LOCAL_EOL
		"bUseUnrealDistortion %d" LOCAL_EOL
		"bHmdEnabled %d" LOCAL_EOL
		"bStereoEnabled %d" LOCAL_EOL
	, bUseUnrealDistortion, bHmdEnabled, bStereoEnabled);
#undef LOCAL_EOL
}

FWaveVRHMD::~FWaveVRHMD()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	Shutdown();
}

void ReportWVRError(WVR_InitError error)
{
	switch (error)
	{
		case WVR_InitError_None:
			break;
		case WVR_InitError_NotInitialized:
			LOGD(WVRHMD, "WaveVR: Not initialized");
			break;
		case WVR_InitError_Unknown:
			LOGD(WVRHMD, "WaveVR: Unknown error during initializing");
			break;
		default:
			//LOGD(WVRHMD, "WaveVR: InitError default case");
			break;
	}
}

void ReportRenderError(WVR_RenderError render_error)
{
	switch (render_error)
	{
		case WVR_RenderError_None:
			LOGD(WVRHMD, "WaveVR: RenderError: None");
			break;
		case WVR_RenderError_RuntimeInitFailed:
			LOGD(WVRHMD, "WaveVR: RenderError: RuntimeInitFailed");
			break;
		case WVR_RenderError_ContextSetupFailed:
			LOGD(WVRHMD, "WaveVR: RenderError: ContextSetupFailed");
			break;
		case WVR_RenderError_DisplaySetupFailed:
			LOGD(WVRHMD, "WaveVR: RenderError: DisplaySetupFailed");
			break;
		case WVR_RenderError_LibNotSupported:
			LOGD(WVRHMD, "WaveVR: RenderError: LibNotSupported");
			break;
		case WVR_RenderError_NullPtr:
			LOGD(WVRHMD, "WaveVR: RenderError: NullPtr");
			break;
		default:
			//LOGD(WVRHMD, "WaveVR: RenderError default case");
			break;
	}
}

void FWaveVRHMD::setDeviceSupportedMaxFPS()
{
	LOG_FUNC();
	WVR_RenderProps props;
	if (WVR()->GetRenderProps(&props)) {
		float fps = props.refreshRate;
		LOGI(WVRHMD, "Set FreshRate as %f", fps);
		GEngine->SetMaxFPS(fps);  // If set to 500, the tick frequency will become 500 if possible.
		//IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS"));
		//CVar->Set(fps);
	}
	else {
		LOGW(WVRHMD, "Get device FreshRate error! Not success!");
	}
}

// This is only used in FWaveVRHMD constructor
bool FWaveVRHMD::Startup()
{
	LOG_FUNC();

	FrameData = FFrameData::NewInstance();
	FrameDataRT = FFrameData::NewInstance();
	OldFrameData = FFrameData::NewInstance();
	OldFrameDataRT = FFrameData::NewInstance();

	// load WaveVR project settings from ini
	ApplyCVarSettingsFromIni(TEXT("/Script/WaveVREditor.WaveVRSettings"), *GEngineIni, ECVF_SetByProjectSetting);

	auto CVarAQEnable = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("wvr.AdaptiveQuality"));
	bAdaptiveQuality = CVarAQEnable && CVarAQEnable->GetValueOnAnyThread() != 0;

	WVR_InitError error = WVR()->Init(WVR_AppType_VRContent);

	if (error != WVR_InitError_None)
	{
		ReportWVRError(error);
		WVR()->Quit();
		LOGD(WVRHMD, "WVR_Quit");
		return false;
	}

	supportedFeatures = WVR()->GetSupportedFeatures();
	LOGI(WVRHMD, "Startup() supportedFeatures: %d", (int)supportedFeatures);
	pollEvent(); //get device connect info.
#if WITH_EDITOR
	if (!GIsEditor)
#endif
	{
		FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddRaw(this, &FWaveVRHMD::ApplicationWillEnterBackgroundDelegate);
		//calls to this delegate are often (always?) paired with a call to ApplicationWillEnterBackgroundDelegate(), but cover the possibility that only this delegate is called
		FCoreDelegates::ApplicationWillDeactivateDelegate.AddRaw(this, &FWaveVRHMD::ApplicationWillDeactivateDelegate);
		FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddRaw(this, &FWaveVRHMD::ApplicationHasEnteredForegroundDelegate);
		//calls to this delegate are often (always?) paired with a call to ApplicationHasEnteredForegroundDelegate(), but cover the possibility that only this delegate is called
		FCoreDelegates::ApplicationHasReactivatedDelegate.AddRaw(this, &FWaveVRHMD::ApplicationHasReactivatedDelegate);
		FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FWaveVRHMD::OnFEngineLoopInitComplete);
		//Handle PreLoadMap and PostLoadMap
		FCoreUObjectDelegates::PreLoadMap.AddRaw(this, &FWaveVRHMD::OnPreLoadMap);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &FWaveVRHMD::OnPostLoadMap);
	}

	NextFrameData();
	if (FrameData->bSupportLateUpdate)
	{
		// LateUpdate will need first frame's pose.
		FrameData->bDoUpdateInGT = true;
		PoseMngr->UpdatePoses(FrameData);
	}

	NearClippingPlane = GNearClippingPlane;
	ResetProjectionMats();

	if (bUseUnrealDistortion)
		SetNumOfDistortionPoints(40, 40);

	//UGameUserSettings* Settings = GetGameUserSettings();
	//if (Settings != nullptr)
	//{
	//	LOGD(WVRHMD, "RequestResolutionChange")
	//	{
	//		uint32 width = 0, height = 0;
	//		Settings->RequestResolutionChange(width * 2, height, EWindowMode::Type::Windowed, false);
	//	}
	//}

	uint32 width = 1024, height = 1024;
	WVR()->GetRenderTargetSize(&width, &height);
	mRender.SetSingleEyePixelSize(width, height);
	mRender.SetTextureFormat(PF_R8G8B8A8);
	mRender.Apply();

	Instance = this;

#if WITH_EDITOR
	if (!GIsEditor)
#endif
	{
		WaveVRSplash = MakeShareable(new FWaveVRSplash(&mRender));
		WaveVRSplash->Init();
	}

	LOGD(WVRHMD, "bAdaptiveQuality is %d", bAdaptiveQuality);
	if (bAdaptiveQuality) {
		WVR()->EnableAdaptiveQuality(bAdaptiveQuality);
		if (WVR()->IsAdaptiveQualityEnabled())
			LOGD(WVRHMD, "AdaptiveQuality is enabled");
	}

	return true;
}

void FWaveVRHMD::OnPause()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	if (!bResumed)
		return;
	bResumed = false;
	mRender.OnPause();
}

void FWaveVRHMD::OnResume()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	if (bResumed)
		return;
	bResumed = true;
	UWaveVREventCommon::OnResumeNative.Broadcast();

	bIsHmdConnected = PoseMngr->IsDeviceConnected(WVR_DeviceType::WVR_DeviceType_HMD);
	bIsRightDeviceConnected = PoseMngr->IsDeviceConnected(WVR_DeviceType::WVR_DeviceType_Controller_Right);
	bIsLeftDeviceConnected = PoseMngr->IsDeviceConnected(WVR_DeviceType::WVR_DeviceType_Controller_Left);

	mRender.OnResume();
}

void FWaveVRHMD::ApplicationWillEnterBackgroundDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnPause();
}

void FWaveVRHMD::ApplicationWillDeactivateDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnPause();
}

void FWaveVRHMD::ApplicationHasReactivatedDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnResume();
}

void FWaveVRHMD::ApplicationHasEnteredForegroundDelegate()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	OnResume();
}

void FWaveVRHMD::OnFEngineLoopInitComplete()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	setDeviceSupportedMaxFPS();
}

void FWaveVRHMD::OnPreLoadMap(const FString&)
{
	LOG_FUNC_IF(1);
	//Force disable AdaptiveQuality to increase performance during load map.
	FWaveVRAPIWrapper::GetInstance()->EnableAdaptiveQuality(false);
}

void FWaveVRHMD::OnPostLoadMap(UWorld*)
{
	LOG_FUNC_IF(1);
	//Restore AdaptiveQuality status.
	FWaveVRAPIWrapper::GetInstance()->EnableAdaptiveQuality(bAdaptiveQuality);
}

void FWaveVRHMD::Shutdown()
{
	LOG_FUNC_IF(WAVEVR_LOG_ENTRY_LIFECYCLE);
	if (!GIsEditor)
	{
		LOGI(WVRHMD, "Stop Hand Gesture before WVR_Quit.");
		StopHandGesture();
		LOGI(WVRHMD, "Stop Hand Tracking before WVR_Quit.");
		StopHandTracking();
		mRender.Shutdown();
		WVR()->Quit();
	} else {

		if (DistortionMeshVerticesLeftEye != nullptr)
			delete[] DistortionMeshVerticesLeftEye;
		DistortionMeshVerticesLeftEye = nullptr;

		if (DistortionMeshVerticesRightEye != nullptr)
			delete[] DistortionMeshVerticesRightEye;
		DistortionMeshVerticesRightEye = nullptr;

		if (DistortionMeshIndices != nullptr)
			delete[] DistortionMeshIndices;
		DistortionMeshIndices = nullptr;

		if (bSIM_Available)
			WVR()->Quit();
	}

	Instance = nullptr;
}

static FMatrix MakeProjection(float Left, float Right, float Top, float Bottom, float ZNear, float ZFar = 0) {
	LOGI(WVRHMD, "MakeProjection (%f, %f, %f, %f, %f, %f)", Left, Right, Top, Bottom, ZNear, ZFar);

	float SumRL = (Right + Left);
	float SumTB = (Top + Bottom);
	float SubRL = Right - Left;
	float SubTB = Top - Bottom;

	if (ZFar > 0 && ZNear > 0 && ZFar > ZNear) {
		// Reversed Z [1, 0].  Use this function if ZFar is finite.
		return FMatrix(
			FPlane(2.0f / SubRL, 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 2.0f / SubTB, 0.0f, 0.0f),
			FPlane(SumRL / -SubRL, SumTB / -SubTB, ZNear / (ZNear - ZFar), 1.0f),
			FPlane(0.0f, 0.0f, ZFar * ZNear / (ZFar - ZNear), 0.0f)
		);
	}
	else
	{
		// Reversed Z [1, 0].  Use this function if ZFar is infinite.
		return FMatrix(
			FPlane(2.0f / SubRL, 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 2.0f / SubTB, 0.0f, 0.0f),
			FPlane(SumRL / -SubRL, SumTB / -SubTB, 0.0f, 1.0f),
			FPlane(0.0f, 0.0f, ZNear, 0.0f)
		);
	}
}

void FWaveVRHMD::ResetProjectionMats()
{
	LOG_FUNC();
	bNeedResetProjectionMatrix = false;

	float boundaries[3][4];
	WVR()->GetClippingPlaneBoundary(WVR_Eye_Left, &(boundaries[0][0]), &(boundaries[0][1]), &(boundaries[0][2]), &(boundaries[0][3]));
	WVR()->GetClippingPlaneBoundary(WVR_Eye_Right, &(boundaries[1][0]), &(boundaries[1][1]), &(boundaries[1][2]), &(boundaries[1][3]));
	boundaries[2][0] = FMath::Min(boundaries[0][0], boundaries[1][0]);
	boundaries[2][1] = FMath::Max(boundaries[0][1], boundaries[1][1]);
	boundaries[2][2] = FMath::Max(boundaries[0][2], boundaries[1][2]);
	boundaries[2][3] = FMath::Min(boundaries[0][3], boundaries[1][3]);

	LeftProjection = MakeProjection(boundaries[0][0], boundaries[0][1], boundaries[0][2], boundaries[0][3], NearClippingPlane, FarClippingPlane);
	RightProjection = MakeProjection(boundaries[1][0], boundaries[1][1], boundaries[1][2], boundaries[1][3], NearClippingPlane, FarClippingPlane);
	CenterProjection = MakeProjection(boundaries[2][0], boundaries[2][1], boundaries[2][2], boundaries[2][3], NearClippingPlane, FarClippingPlane);
}

void FWaveVRHMD::SetClippingPlanes(float NCP, float FCP) {
	LOG_FUNC();
	NearClippingPlane = FMath::Max(NCP, 1.0f);
	FarClippingPlane = FMath::Max(FCP, 0.0f);

	GNearClippingPlane = NearClippingPlane;

	bNeedResetProjectionMatrix = true;

	// Do we need reset it immediatly?
	ResetProjectionMats();
}

void FWaveVRHMD::SetInAppRecenter(bool enabled) {
	LOG_FUNC();
	bIsInAppRecenter = enabled;
	if (bIsInAppRecenter == true) {
		FrameData->basePosition = CompensationOffset = FVector::ZeroVector;
		FrameData->baseOrientation = BaseOrientation = FQuat::Identity;
	}
}

bool FWaveVRHMD::IsInAppRecenter() {
	LOG_FUNC();
	return bIsInAppRecenter;
}

bool FWaveVRHMD::IsDirectPreview() {
	LOG_FUNC();
	return bSIM_Available;
}

void FWaveVRHMD::EnableFoveation(bool enable) {
	mRender.SetFoveatedRendering(enable);
}

void FWaveVRHMD::SetFoveationParams(EStereoscopicPass Eye, WVR_RenderFoveationParams_t& FoveatParams) {
	mRender.SetFoveationParams(Eye, FoveatParams);
}

bool FWaveVRHMD::IsSplashShowing() {
	if (mRender.IsInitialized() && mRender.IsCustomPresentSet() && WaveVRSplash.IsValid()) {
		return WaveVRSplash->IsShown();
	}
	return false;
}

FWaveVRHMD::FWaveSceneViewExtension::FWaveSceneViewExtension(const FAutoRegister& AutoRegister)
: FSceneViewExtensionBase(AutoRegister) {}

void FWaveVRHMD::FWaveSceneViewExtension::PostRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily)
{
	LOG_FUNC();
#if WITH_EDITOR
	if (DirectPreview != nullptr && GetInstance()->IsDirectPreview()) {
#ifdef DP_DEBUG
		LOGW(WVRHMD, "PostRenderViewFamily_RenderThread %d", x2++);
#endif
		FRHITexture2D* TexRef2D = InViewFamily.RenderTarget->GetRenderTargetTexture()->GetTexture2D();
		uint32 LolStride = 0;
		char* TextureDataPtr = (char*)RHICmdList.LockTexture2D(TexRef2D, 0, EResourceLockMode::RLM_ReadOnly, LolStride, false);
		DirectPreview->sendRTTexture(RHICmdList, TexRef2D->GetNativeResource());
		RHICmdList.UnlockTexture2D(TexRef2D, 0, false);
	}
#endif
}

bool FWaveVRHMD::IsRenderInitialized() {
	return mRender.IsInitialized();
}


void FWaveVRHMD::SimulateCPULoading(unsigned int gameThreadLoading, unsigned int renderThreadLoading) {
#if (UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG)
	LOGW(WVRHMD, "Using Sleep to simulate CPU loading: GT %7.3fms, RT %7.3fms", gameThreadLoading / 1000.0f, renderThreadLoading / 1000.0f);
	SimulatedLoadingGameThread = FMath::Clamp<unsigned int>(gameThreadLoading, 0, 100000);
	SimulatedLoadingRenderThread = FMath::Clamp<unsigned int>(renderThreadLoading, 0, 100000);
#endif
}
