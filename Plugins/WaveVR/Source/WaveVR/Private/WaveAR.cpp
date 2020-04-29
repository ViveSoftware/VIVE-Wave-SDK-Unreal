// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveAR.h"
#include "WaveVRPrivatePCH.h"
#include "WaveVRHMD.h"
#include "IXRTrackingSystem.h"

#include "ARSessionConfig.h"
#include "ARPin.h"
#include "ARTraceResult.h"

FWaveAR* FWaveAR::GetInstance()
{
	static FWaveAR* mInst = new FWaveAR();
	return mInst;
}

FWaveAR::FWaveAR() :
	bHasValidPose(false),
	CachedPosition(FVector::ZeroVector),
	CachedOrientation(FQuat::Identity),
	LightEstimate(nullptr)
{
}

FWaveAR::~FWaveAR(){
}

UARBasicLightEstimate* FWaveAR::GetLightEstimate()
{
	return LightEstimate;
}

TArray<UARTrackedGeometry*> FWaveAR::GetCachedAllTrackedGeometry() const
{
	return CachedAllTrackedGeometry;
}

EARTrackingQuality FWaveAR::GetTrackingQuality() const
{
	if (!bHasValidPose)
	{
		return EARTrackingQuality::NotTracking;
	}

	return EARTrackingQuality::OrientationAndPosition;
}

void FWaveAR::UpdateData(){

	bHasValidPose = false;
	FQuat CurRotation;
	FVector CurTranslation;
	bHasValidPose = GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, CurRotation, CurTranslation);
	FTransform CurPose(CurRotation, CurTranslation);
	//CurPose *= GetAlignmentTransform();

	if (bHasValidPose)
	{
		CachedOrientation = CurPose.GetRotation();
		CachedPosition = CurPose.GetTranslation();
	}

	if (LightEstimate == nullptr)
	{
		LightEstimate = NewObject<UARBasicLightEstimate>();
	}
	//TODO : Set LightEstimate

	CreateFakePlane();
}

void FWaveAR::CreateFakePlane() {
	LOG_FUNC();

	//FTransform FakeTransform = FTransform(FQuat::Identity, FVector(100, 0, -100));
	//FTransform HMDTransform = FTransform(CachedOrientation, CachedPosition);
	//FTransform HMDTransformInverse = HMDTransform.Inverse();
	//FTransform FakeTransformLocalToTracking;
	//FTransform::Multiply(&FakeTransformLocalToTracking, &HMDTransformInverse, &FakeTransform);

	//CachedAllTrackedGeometry.Empty();
	//UARPlaneGeometry* PlaneObject = NewObject<UARPlaneGeometry>();
	//PlaneObject->UpdateTrackedGeometry(
	//	FWaveVRHMD::GetARSystem().ToSharedRef(),//const TSharedRef < FARSupportInterface , ESPMode::ThreadSafe > & InTrackingSystem,
	//	0,                      //uint32 FrameNumber,
	//	0,                      //double Timestamp,
	//	FakeTransform,          //FakeTransformLocalToTracking, //const FTransform & InLocalToTrackingTransform, //TODO: This should get from runtime, now we fake it as fixed transform.
	//	FTransform::Identity,   //GetAlignmentTransform(),  //const FTransform & InAlignmentTransform,
	//	FVector(0, 0, 0),       //const FVector InCenter,
	//	FVector(50, 50, 0)      //const FVector InExtent
	//);
	//UARTrackedGeometry* NewTrackableObject = static_cast<UARTrackedGeometry*>(PlaneObject);
	//CachedAllTrackedGeometry.Add(NewTrackableObject);
}

/****************************************************
 *
 * IARSystemSupport
 *
 ****************************************************/

void FWaveVRHMD::OnARSystemInitialized()
{
	LOG_FUNC();
}

EARTrackingQuality FWaveVRHMD::OnGetTrackingQuality() const
{
	LOG_FUNC();
	return mWaveAR->GetTrackingQuality();
}

EARTrackingQualityReason FWaveVRHMD::OnGetTrackingQualityReason() const
{
	LOG_FUNC();
	//TODO
	return EARTrackingQualityReason::None;
}

void FWaveVRHMD::OnStartARSession(UARSessionConfig* SessionConfig)
{
	LOG_FUNC();
	//TODO
}

void FWaveVRHMD::OnPauseARSession()
{
	LOG_FUNC();
	//TODO
}

void FWaveVRHMD::OnStopARSession()
{
	LOG_FUNC();
	//TODO
}

FARSessionStatus FWaveVRHMD::OnGetARSessionStatus() const
{
	LOG_FUNC();
	// TODO
	return EARSessionStatus::Running;
}

bool FWaveVRHMD::IsARAvailable() const
{
	LOG_FUNC();
	// TODO
	return false;
}

void FWaveVRHMD::OnSetAlignmentTransform(const FTransform& InAlignmentTransform)
{
	LOG_FUNC();
	//const FTransform& NewAlignmentTransform = InAlignmentTransform;

	//TArray<UARTrackedGeometry*> AllTrackedGeometries = GetAllTrackedGeometries();
	//for (UARTrackedGeometry* TrackedGeometry : AllTrackedGeometries)
	//{
	//	TrackedGeometry->UpdateAlignmentTransform(NewAlignmentTransform);
	//}

	//TArray<UARPin*> AllARPins = GetAllPins();
	//for (UARPin* SomePin : AllARPins)
	//{
	//	SomePin->UpdateAlignmentTransform(NewAlignmentTransform);
	//}

	//SetAlignmentTransform_Internal(InAlignmentTransform);
}

TArray<FARTraceResult> FWaveVRHMD::OnLineTraceTrackedObjects(const FVector2D ScreenCoord, EARLineTraceChannels TraceChannels)
{
	LOG_FUNC();
	//We don't need to implement this. Always run on VR.
	TArray<FARTraceResult> OutHitResults;
	return OutHitResults;
}

TArray<FARTraceResult> FWaveVRHMD::OnLineTraceTrackedObjects(const FVector Start, const FVector End, EARLineTraceChannels TraceChannels) {
	LOG_FUNC();
	TArray<FARTraceResult> OutHitResults;
	return OutHitResults;
}


TArray<UARTrackedGeometry*> FWaveVRHMD::OnGetAllTrackedGeometries() const
{
	LOG_FUNC();
	return mWaveAR->GetCachedAllTrackedGeometry();
}

TArray<UARPin*> FWaveVRHMD::OnGetAllPins() const
{
	LOG_FUNC();
	TArray<UARPin*> AllARPins;
	// TODO: Update AllARPins.
	return AllARPins;
}

bool FWaveVRHMD::OnIsTrackingTypeSupported(EARSessionType SessionType) const
{
	LOG_FUNC();
	//TODO
	return true;
}

UARLightEstimate* FWaveVRHMD::OnGetCurrentLightEstimate() const
{
	LOG_FUNC();
	return mWaveAR->GetLightEstimate();
}

UARPin* FWaveVRHMD::OnPinComponent(USceneComponent* ComponentToPin, const FTransform& PinToWorldTransform, UARTrackedGeometry* TrackedGeometry /*= nullptr*/, const FName DebugName /*= NAME_None*/)
{
	LOG_FUNC();
	UARPin* NewARPin = nullptr;
	// TODO: Pin component.
	return NewARPin;
}

void FWaveVRHMD::OnRemovePin(UARPin* PinToRemove)
{
	LOG_FUNC();
	// TODO: Remove Pin
}

EARWorldMappingState FWaveVRHMD::OnGetWorldMappingStatus() const
{
	LOG_FUNC();
	return EARWorldMappingState::StillMappingNotRelocalizable;
}

TArray<FVector> FWaveVRHMD::OnGetPointCloud() const
{
	LOG_FUNC();
	return TArray<FVector>();
}

bool FWaveVRHMD::OnAddRuntimeCandidateImage(UARSessionConfig* SessionConfig, UTexture2D* CandidateTexture, FString FriendlyName, float PhysicalWidth) {
	return false;
}

/****************************************************
 *
 * FARSystemBase
 *
 ****************************************************/

void FWaveVRHMD::AddReferencedObjects(FReferenceCollector& Collector)
{
	//LOG_FUNC();
	//FARSystemBase::AddReferencedObjects(Collector);

	//if (mWaveAR->GetLightEstimate() != nullptr)
	//{
	//	UARLightEstimate* LightEstimate = mWaveAR->GetLightEstimate();
	//	Collector.AddReferencedObject(LightEstimate);
	//}
}

void* FWaveVRHMD::GetARSessionRawPointer()
{
	LOG_FUNC();
	ensureAlwaysMsgf(false, TEXT("WaveAR GetARSessionRawPointer is unimplemented on current platform."));
	return nullptr;
}

void* FWaveVRHMD::GetGameThreadARFrameRawPointer()
{
	LOG_FUNC();
	ensureAlwaysMsgf(false, TEXT("WaveAR GetARSessionRawPointer is unimplemented on current platform."));
	return nullptr;
}
