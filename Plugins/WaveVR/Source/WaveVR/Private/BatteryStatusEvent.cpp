// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "BatteryStatusEvent.h"
#include "WaveVRPrivatePCH.h"

FBatteryStatusDelNative UBatteryStatusEvent::onBatteryStatusUpdateNative;

// Sets default values for this component's properties
UBatteryStatusEvent::UBatteryStatusEvent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBatteryStatusEvent::BeginPlay()
{
	Super::BeginPlay();

	UBatteryStatusEvent::onBatteryStatusUpdateNative.AddDynamic(this, &UBatteryStatusEvent::receiveStatusUpdateFromNative);
}

void UBatteryStatusEvent::receiveStatusUpdateFromNative() {
	LOGW(LogTemp,"receiveStatusUpdateFromNative in UBatteryStatusEvent");

	WaveVR_onBatteryStatusUpdate.Broadcast();
}

// Called every frame
void UBatteryStatusEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

