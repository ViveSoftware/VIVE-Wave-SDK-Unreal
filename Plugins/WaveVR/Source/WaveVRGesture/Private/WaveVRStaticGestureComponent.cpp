// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "WaveVRStaticGestureComponent.h"
#include "WaveVRGestureBPLibrary.h"

FStaticGestureNative UWaveVRStaticGestureComponent::OnStaticGestureNative_Right;
FStaticGestureNative UWaveVRStaticGestureComponent::OnStaticGestureNative_Left;

// Sets default values for this component's properties
UWaveVRStaticGestureComponent::UWaveVRStaticGestureComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UWaveVRStaticGestureComponent::BeginPlay()
{
	Super::BeginPlay();

	UWaveVRStaticGestureComponent::OnStaticGestureNative_Right.AddDynamic(this, &UWaveVRStaticGestureComponent::OnStaticGestureHandling_Right);
	UWaveVRStaticGestureComponent::OnStaticGestureNative_Left.AddDynamic(this, &UWaveVRStaticGestureComponent::OnStaticGestureHandling_Left);

	if (!UWaveVRGestureBPLibrary::IsHandGestureAvailable())
	{
		UWaveVRGestureBPLibrary::StartHandGesture();
		UE_LOG(LogWaveVRStaticGestureComponent, Log, TEXT("BeginPlay() Start hand gesture."));
	}
}

void UWaveVRStaticGestureComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWaveVRGestureBPLibrary::IsHandGestureAvailable())
	{
		UWaveVRGestureBPLibrary::StopHandGesture();
		UE_LOG(LogWaveVRStaticGestureComponent, Log, TEXT("EndPlay() Stop hand gesture."));
	}
}

// Called every frame
void UWaveVRStaticGestureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

#pragma region
void UWaveVRStaticGestureComponent::OnStaticGestureHandling_Right(EWaveVRStaticGesture type)
{
	UE_LOG(LogWaveVRStaticGestureComponent, Log, TEXT("OnStaticGestureHandling_Right() type: %d"), (uint8)type);
	OnStaticGestureBp_Right.Broadcast(type);
}

void UWaveVRStaticGestureComponent::OnStaticGestureHandling_Left(EWaveVRStaticGesture type)
{
	UE_LOG(LogWaveVRStaticGestureComponent, Log, TEXT("OnStaticGestureHandling_Left() type: %d"), (uint8)type);
	OnStaticGestureBp_Left.Broadcast(type);
}
#pragma endregion Gesture events
