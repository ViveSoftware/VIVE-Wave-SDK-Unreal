// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRSkeletonBone.h"

// Sets default values for this component's properties
UWaveVRSkeletonBone::UWaveVRSkeletonBone()
	: location(FVector::ZeroVector)
	, rotation(FRotator::ZeroRotator)
	, validPose(false)
	, frameCount(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	UE_LOG(LogWaveVRSkeletonBone, Log, TEXT("UWaveVRSkeletonBone() Enabled = %d, Bond ID = %d"), (uint8)Enabled, (uint8)BoneId);
}


// Called when the game starts
void UWaveVRSkeletonBone::BeginPlay()
{
	Super::BeginPlay();

	if (Enabled)
	{
		if (!UWaveVRGestureBPLibrary::IsHandTrackingAvailable())
		{
			UWaveVRGestureBPLibrary::StartHandTracking();
			UE_LOG(LogWaveVRSkeletonBone, Log, TEXT("BeginPlay() Start hand tracking. Bone %d."), (uint8)BoneId);
		}
	}
}

void UWaveVRSkeletonBone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWaveVRGestureBPLibrary::IsHandTrackingAvailable())
	{
		UWaveVRGestureBPLibrary::StopHandTracking();
		UE_LOG(LogWaveVRSkeletonBone, Log, TEXT("EndPlay() Stop hand tracking. Bone %d."), (uint8)BoneId);
	}
}

uint8 boneTick = 0;
// Called every frame
void UWaveVRSkeletonBone::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UWaveVRGestureBPLibrary::IsHandTrackingAvailable())
	{
		validPose = UWaveVRGestureBPLibrary::GetBonePositionAndRotation(BoneId, location, rotation);

		if (validPose)
		{
			boneTick++;
			boneTick %= 300;
			if (boneTick == 0)
			{
				UE_LOG(LogWaveVRSkeletonBone, Log, TEXT("Bone %d position (%f, %f, %f)."), (uint8)BoneId, location.X, location.Y, location.Z);
			}
		}
	}
	else
	{
		validPose = false;
	}
}

FVector UWaveVRSkeletonBone::GetLocation()
{
	return location;
}

FRotator UWaveVRSkeletonBone::GetRotation()
{
	return rotation;
}

bool UWaveVRSkeletonBone::IsValidPose()
{
	return validPose;
}
