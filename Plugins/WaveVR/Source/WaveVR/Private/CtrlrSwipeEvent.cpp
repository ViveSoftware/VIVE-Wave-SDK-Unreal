// "WaveVR SDK
// © 2019 HTC Corporation. All Rights Reserved.
//
// Unless otherwise required by copyright law and practice,
// upon the execution of HTC SDK license agreement,
// HTC grants you access to and use of the WaveVR SDK(s).
// You shall fully comply with all of HTC’s SDK license agreement terms and
// conditions signed by you and all SDK and API requirements,
// specifications, and documentation provided by HTC to You."

#include "CtrlrSwipeEvent.h"
#include "WaveVRPrivatePCH.h"

FCtrlrSwipeLRDelNative UCtrlrSwipeEvent::onCtrlrSwipeLtoRUpdateNative;
FCtrlrSwipeRLDelNative UCtrlrSwipeEvent::onCtrlrSwipeRtoLUpdateNative;
FCtrlrSwipeUDDelNative UCtrlrSwipeEvent::onCtrlrSwipeUtoDUpdateNative;
FCtrlrSwipeDUDelNative UCtrlrSwipeEvent::onCtrlrSwipeDtoUUpdateNative;

// Sets default values for this component's properties
UCtrlrSwipeEvent::UCtrlrSwipeEvent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCtrlrSwipeEvent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UCtrlrSwipeEvent::onCtrlrSwipeLtoRUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeLRUpdateFromNative);
	UCtrlrSwipeEvent::onCtrlrSwipeRtoLUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeRLUpdateFromNative);
	UCtrlrSwipeEvent::onCtrlrSwipeUtoDUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeUDUpdateFromNative);
	UCtrlrSwipeEvent::onCtrlrSwipeDtoUUpdateNative.AddDynamic(this, &UCtrlrSwipeEvent::receiveCtrlrSwipeDUUpdateFromNative);
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeLRUpdateFromNative() {
	LOGW(LogTemp, "receiveCtrlrSwipeLRUpdateFromNative in UCtrlrSwipeEvent");

	WaveVR_onCtrlrSwipeLRUpdate.Broadcast();
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeRLUpdateFromNative() {
	LOGW(LogTemp, "receiveCtrlrSwipeRLUpdateFromNative in UCtrlrSwipeEvent");

	WaveVR_onCtrlrSwipeRLUpdate.Broadcast();
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeUDUpdateFromNative() {
	LOGW(LogTemp, "receiveCtrlrSwipeUDUpdateFromNative in UCtrlrSwipeEvent");

	WaveVR_onCtrlrSwipeUDUpdate.Broadcast();
}

void UCtrlrSwipeEvent::receiveCtrlrSwipeDUUpdateFromNative() {
	LOGW(LogTemp, "receiveCtrlrSwipeDUUpdateFromNative in UCtrlrSwipeEvent");

	WaveVR_onCtrlrSwipeDUUpdate.Broadcast();
}

// Called every frame
void UCtrlrSwipeEvent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

